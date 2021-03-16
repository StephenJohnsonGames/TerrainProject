
#include "pch.h"
#include <thread>
#include "src/ArcballCamera.hpp"
#include "src/AIScene.hpp"
#include "src/GUClock.hpp"
#include "src/SkyBox.hpp"
#include "src/ReflectiveMesh.hpp"


using namespace std;
using namespace cst;


#pragma region Scene variables

GUClock* mainClock = nullptr;
ArcballCamera* mainCamera = nullptr;

// Variables needed to track where the mouse pointer is so we can determine which direction it's moving in
int mouse_x, mouse_y;
bool mDown = false;

SkyBox* skybox = nullptr;
AIScene* sphereModel = nullptr;
ReflectiveMesh* reflectiveSphere = nullptr;

#pragma endregion


#pragma region Function Prototypes

void init(int argc, char* argv[]); // Main scene initialisation function
void update(void); // Main scene update function
void display(void); // Main scene render function
void resizeWindow(int x, int y);
// Event handling functions
void mouseButtonDown(int button_id, int state, int x, int y);
void mouseMove(int x, int y);
void mouseWheel(int wheel, int direction, int x, int y);
void keyDown(unsigned char key, int x, int y);


#pragma endregion


int main(int argc, char* argv[])
{
	mainClock = new GUClock();

	init(argc, argv);
	glutMainLoop();

	if (mainClock) {

		mainClock->stop();
		mainClock->reportTimingData();
	}

	return 0;
}


void init(int argc, char* argv[]) {

	// Initialise FreeGLUT
	glutInit(&argc, argv);

	glutInitContextVersion(4, 6);
	//glutInitContextProfile(GLUT_COMPATIBILITY_PROFILE);
	glutInitContextProfile(GLUT_CORE_PROFILE);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DEPTH | GLUT_DOUBLE | GLUT_MULTISAMPLE);
	glutSetOption(GLUT_MULTISAMPLE, 4);

	// Setup window
	int windowWidth = 1920;
	int windowHeight = 1080;
	glutInitWindowSize(windowWidth, windowHeight);
	glutInitWindowPosition(64, 64);
	glutCreateWindow("Skybox");
	glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_GLUTMAINLOOP_RETURNS);

	// Register callback functions
	glutIdleFunc(update); // Main scene update function
	glutDisplayFunc(display); // Main render function
	glutReshapeFunc(resizeWindow); // Main window resize handler
	glutKeyboardFunc(keyDown); // Key down handler
	glutMouseFunc(mouseButtonDown); // Mouse button handler
	glutMotionFunc(mouseMove); // Mouse move handler
	glutMouseWheelFunc(mouseWheel); // Mouse wheel event handler

	// Initialise glew
	auto glewStatus = glewInit();
	if (glewStatus != GLEW_OK) {
		cout << "GLEW error" << endl;
	}

	// Report context version
	GLint majorVersion, minorVersion;

	glGetIntegerv(GL_MAJOR_VERSION, &majorVersion);
	glGetIntegerv(GL_MINOR_VERSION, &minorVersion);
	cout << "OpenGL version: " << majorVersion << "." << minorVersion << endl;

	auto glslVersion = glGetString(GL_SHADING_LANGUAGE_VERSION);
	cout << "Shading Language Version: " << glslVersion << endl;

	auto vendorStr = glGetString(GL_VENDOR);
	cout << "Vendor: " << vendorStr << endl;

	auto rendererString = glGetString(GL_RENDERER);
	cout << "Renderer: " << rendererString << endl;

	//thread showExtensions(showExtensionsFn);
	// Note: would like to list extensions in another thread, but gl context lives in main thread!
	GLint numExtensions;
	glGetIntegerv(GL_NUM_EXTENSIONS, &numExtensions);
	cout << "Extensions (" << numExtensions << ")..." << endl;
	for (GLint i = 0; i < numExtensions; i++) {

		cout << (const char*)glGetStringi(GL_EXTENSIONS, i) << endl;
	}
	cout << "\n\n";


	// Initialise OpenGL...

	wglSwapIntervalEXT(0);

	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glFrontFace(GL_CCW); // Default anyway

	// Setup colour to clear the window
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);


	// glm tests...
	/*
	glm::mat4 T = glm::translate(glm::mat4(1.0f), glm::vec3(3.0f, 0.0f, 0.0f));
	glm::mat4 R = glm::rotate(glm::mat4(1.0f), 45.0f * cst::radian<float>(), glm::vec3(0.0f, 0.0f, 1.0f));

	glm::vec4 v1 = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
	glm::vec4 v1_ = T * R * v1;

	cout << glm::to_string(v1_) << endl;

	auto a = glm::radians<float>(90.0f);
	cout << "90 degrees in radians = " << a;
	cout << a << " in degrees = " << glm::degrees<float>(a) << endl;

	glm::mat4 Rx = glm::eulerAngleX(-4.5f);
	glm::mat4 Ry = glm::eulerAngleY(2.1f);
	glm::mat4 Rz = glm::eulerAngleZ(3.7f);

	glm::mat4 Rxyz = Rz * Ry * Rx;
	glm::mat4 Rxyz_ = glm::eulerAngleZYX(3.7f, 2.1f, -4.5f);

	cout << glm::to_string(Rxyz) << endl;
	cout << glm::to_string(Rxyz_) << endl;
	*/


	// Setup main camera
	float viewportAspect = (float)windowWidth / (float)windowHeight;
	mainCamera = new ArcballCamera(0.0f, 0.0f, 5.0f, 55.0f, viewportAspect, 0.1f, 1000.0f);

	sphereModel = new AIScene("..\\shared_resources\\Models\\gsphere.obj", aiProcess_GenNormals | aiProcess_CalcTangentSpace | aiProcess_Triangulate | aiProcess_JoinIdenticalVertices | aiProcess_SortByPType);
	
	skybox = new SkyBox(string("Resources\\Textures\\cubemap_snow\\"), string("snow"), string(".png"));

	reflectiveSphere = new ReflectiveMesh(sphereModel, skybox->skyboxTexture());


}

// Main scene update function (called by FreeGLUT's main event loop every frame) 
void update(void) {

	mainClock->tick();

	// Redraw the screen
	display();

	// Update the window title to show current frames-per-second and seconds-per-frame data
	char timingString[256];
	sprintf_s(timingString, 256, "SJ_Cubemap: Average fps: %.0f; Average spf: %f", mainClock->averageFPS(), mainClock->averageSPF() / 1000.0f);
	glutSetWindowTitle(timingString);
}

void display(void) {

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Set viewport to the client area of the current window
	glViewport(0, 0, glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT));

	// Get view-projection transform as a GUMatrix4
	glm::mat4 T = mainCamera->projectionTransform() * mainCamera->viewTransform();

	if (skybox)
		skybox->render(T);

	if (reflectiveSphere)
		reflectiveSphere->render(mainCamera);

	glutSwapBuffers();
}

void resizeWindow(int x, int y) {

	mainCamera->setAspectRatio(float(glutGet(GLUT_WINDOW_WIDTH)) / float(glutGet(GLUT_WINDOW_HEIGHT)));
}


#pragma region Event handling functions

void mouseButtonDown(int button_id, int state, int x, int y) {

	if (button_id == GLUT_LEFT_BUTTON) {

		if (state == GLUT_DOWN) {

			mouse_x = x;
			mouse_y = y;

			mDown = true;

		}
		else if (state == GLUT_UP) {

			mDown = false;
		}
	}
}


void mouseMove(int x, int y) {

	int dx = x - mouse_x;
	int dy = y - mouse_y;

	if (mainCamera)
		mainCamera->rotateCamera((float)-dy, (float)-dx);

	mouse_x = x;
	mouse_y = y;
}


void mouseWheel(int wheel, int direction, int x, int y) {

	if (mainCamera) {

		if (direction < 0)
			mainCamera->scaleRadius(1.1f);
		else if (direction > 0)
			mainCamera->scaleRadius(0.9f);
	}
}


void keyDown(unsigned char key, int x, int y) {

	// Toggle fullscreen (This does not adjust the display mode however!)
	if (key == 'f') {

		glutFullScreenToggle();
	}
}

#pragma endregion

