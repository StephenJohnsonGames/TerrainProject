#include "pch.h"
#include "src/ArcballCamera.hpp"
#include "src/PrincipleAxesModel.hpp"
#include "src/TexturedQuadModel.hpp"
#include "src/AIScene.hpp"
#include "src/GUClock.hpp"
#include "src/GUFont.hpp"

using namespace std;
using namespace cst;

#pragma region Scene variables

GUClock* mainClock = nullptr;
ArcballCamera*		mainCamera = nullptr;

// Variables needed to track where the mouse pointer is so we can determine which direction it's moving in
int					mouse_x, mouse_y;
bool				mDown = false;

PrincipleAxesModel*	principleAxes = nullptr;

static const GLuint				NUM_ROADS = 5;

TexturedQuadModel*				road[NUM_ROADS];
int								currentRoad;

GUFont*							font;
glm::mat4						fontViewMatrix;
glm::vec4						fontColour;

#pragma endregion


#pragma region Function Prototypes

void init(int argc, char* argv[]); // Main scene initialisation function
void update(void); // Main scene update function
void display(void); // Main scene render function

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
	glutInitContextProfile(GLUT_COMPATIBILITY_PROFILE);
	//glutInitContextProfile(GLUT_CORE_PROFILE);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DEPTH | GLUT_DOUBLE/* | GLUT_MULTISAMPLE*/);
	//glutSetOption(GLUT_MULTISAMPLE, 4);

	// Setup window
	int windowWidth = 1920;
	int windowHeight = 1080;
	glutInitWindowSize(windowWidth, windowHeight);
	glutInitWindowPosition(64, 64);
	glutCreateWindow("TextureFiltering");
	glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_GLUTMAINLOOP_RETURNS);

	// Register callback functions
	glutIdleFunc(update); // Main scene update function
	glutDisplayFunc(display); // Main render function
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
	int majorVersion, minorVersion;

	glGetIntegerv(GL_MAJOR_VERSION, &majorVersion);
	glGetIntegerv(GL_MINOR_VERSION, &minorVersion);
	cout << "OpenGL version " << majorVersion << "." << minorVersion << "\n\n";

	auto vendorStr = glGetString(GL_VENDOR);
	cout << "Vendor = " << vendorStr << endl;

	// Report extensions
	/*cout << "Extensions supported...\n\n";

	const char* glExtensionString = (const char*)glGetString(GL_EXTENSIONS);

	char* strEnd = (char*)glExtensionString + strlen(glExtensionString);
	char* sptr = (char*)glExtensionString;

	while (sptr < strEnd) {

		int slen = (int)strcspn(sptr, " ");
		printf("%.*s\n", slen, sptr);
		sptr += slen + 1;
	}*/

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

	// Setup font
	font = new GUFont(wstring(L"Courier New"), 18);
	fontViewMatrix = glm::ortho(-4.0f, 4.0f, -4.0f, 4.0f, -1.0f, 1.0f);
	fontColour = glm::vec4(1.0f, 1.0f, 0.0f, 1.0f);


	principleAxes = new PrincipleAxesModel();

	//
	// Load example road texture with different filtering properites
	//

	// Point filtering
	road[0] = new TexturedQuadModel("Resources\\Textures\\road.bmp", TextureProperties(GL_COMPRESSED_SRGB, GL_NEAREST, GL_NEAREST, 1.0f, GL_REPEAT, GL_REPEAT, false, true));

	// Bilinear filtering
	road[1] = new TexturedQuadModel("Resources\\Textures\\road.bmp", TextureProperties(GL_SRGB8_ALPHA8, GL_LINEAR, GL_LINEAR, 1.0f, GL_REPEAT, GL_REPEAT, false, true));

	// Tri-linear filtering
	road[2] = new TexturedQuadModel("Resources\\Textures\\road.bmp", TextureProperties(GL_SRGB8_ALPHA8, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR, 1.0f, GL_REPEAT, GL_REPEAT, true, true));

	// Anisotropic x2
	road[3] = new TexturedQuadModel("Resources\\Textures\\road.bmp", TextureProperties(GL_SRGB8_ALPHA8, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR, 2.0f, GL_REPEAT, GL_REPEAT, true, true));

	// Anisotropic x8
	road[4] = new TexturedQuadModel("Resources\\Textures\\road.bmp", TextureProperties(GL_SRGB8_ALPHA8, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR, 8.0f, GL_REPEAT, GL_REPEAT, true, true));

	currentRoad = 0;

	// Report size of compressed texture
#if 1
	GLint textureSize;
	glBindTexture(GL_TEXTURE_2D, road[0]->getTexture());
	glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_COMPRESSED_IMAGE_SIZE, &textureSize);
	cout << "Road texture compressed with GL_COMPRESSED_SRGB = " << textureSize << endl;
#endif


}

// Main scene update function (called by FreeGLUT's main event loop every frame) 
void update(void) {

	mainClock->tick();

	// Redraw the screen
	display();

	// Update the window title to show current frames-per-second and seconds-per-frame data
	char timingString[256];
	sprintf_s(timingString, 256, "SJ_TextureFiltering: Average fps: %.0f; Average spf: %f", mainClock->averageFPS(), mainClock->averageSPF() / 1000.0f);
	glutSetWindowTitle(timingString);
}

void display(void) {

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Set viewport to the client area of the current window
	glViewport(0, 0, glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT));

	// Get view-projection transform as a GUMatrix4
	glm::mat4 T = mainCamera->projectionTransform() * mainCamera->viewTransform();

	// Get view-projection transform as a GUMatrix4
	glm::mat4 roadMVP =
		T *
		glm::rotate(glm::mat4(1.0f), glm::radians<float>(-80.0f), glm::vec3(1.0f, 0.0f, 0.0f)) *
		glm::scale(glm::mat4(1.0f), glm::vec3(32.0f, 128.0f, 1.0f));

	glEnable(GL_TEXTURE_2D);
	road[currentRoad]->render(roadMVP);


	//
	// Render currently selected filtering mode
	//

	static const char *filterStrings[] = {
		"Point filtering",
		"Bi-linear filtering",
		"Tri-linear filtering",
		"Anisotropic filtering 2x",
		"Anisotropic filtering 8x" };

	glDisable(GL_TEXTURE_2D);
	glUseProgram(0);
	font->renderText(-4.0f, 3.5f, fontViewMatrix, fontColour, "Filtering mode: %s", filterStrings[currentRoad]);
	

	glutSwapBuffers();
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

	switch (key) {

	case '1':
		currentRoad = 0;
		break;

	case '2':
		currentRoad = 1;
		break;

	case '3':
		currentRoad = 2;
		break;

	case '4':
		currentRoad = 3;
		break;

	case '5':
		currentRoad = 4;
		break;

	case 'f':
		glutFullScreenToggle();
		break;
	}

}


#pragma endregion

