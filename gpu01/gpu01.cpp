#include "pch.h"
#include "src/ArcballCamera.hpp"
#include "src/PrincipleAxesModel.hpp"
#include "src/TexturedQuadModel.hpp"
#include "src/PointExtrudeModel.hpp"
#include "src/AIScene.hpp"
#include "src/GUClock.hpp"
#include "src/GrassField.hpp"
#include "src/BasicTerrain.hpp"
#include "src/BasicTessTriangle.hpp"
#include "src/Tessellation/PatchTerrain.hpp"
#include "src/Tessellation/BasicTessTri.hpp"
#include "src/Tessellation/BasicTessQuad.hpp"
#include "src/Tessellation/BasicTessIsoline.hpp"
#include "src/Tessellation/BezierCurveTess.hpp"
#include "src/Tessellation/CatmulRomSpline.hpp"
#include "src/Tessellation/BezierSurface.hpp"
#include "src/Tessellation/BSplineSurface.hpp"
#include "src/ParticleSystemGPU.hpp"
#include "src/ParticleSystemCPU1.hpp"
#include "src/ParticleSystemCPU2.hpp"
#include "src/ProceduralTexture.hpp"
#include "src/Skybox.hpp"


using namespace std;
using namespace cst;

#pragma region Scene variables

GUClock* mainClock = nullptr;
ArcballCamera*		mainCamera = nullptr;
ArcballCamera*		testCamera = nullptr;

// Variables needed to track where the mouse pointer is so we can determine which direction it's moving in
int					mouse_x, mouse_y;
bool				mDown = false;

PrincipleAxesModel*	principleAxes = nullptr;
TexturedQuadModel* texturedQuad = nullptr;
PointExtrudeModel* pExtrude = nullptr;
GrassField* gField = nullptr;
BasicTerrain* bTerrain = nullptr;
PatchTerrain* pTerrain = nullptr;
//BasicTessTriangle* tessTriangle = nullptr;
BasicTessTri* tessTriangle = nullptr;
BasicTessQuad* tessQuad = nullptr;
BasicTessIsoline* tessIsoline = nullptr;
BezierCurveTess* bezierCurve = nullptr;
CatmulRomSpline* crSpline = nullptr;
BezierSurface* bezierSurface = nullptr;
BSplineSurface* bsplineSurface = nullptr;
ParticleSystemGPU* particleSystem = nullptr;
ProceduralTexture* perlinNoiseTexture = nullptr;
SkyBox* skybox = nullptr;

AIScene* sphereModel = nullptr;
GLuint sphereTexture = 0;
GLuint sphereShader = 0;
glm::mat4 sphereTransform = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 1.5f));
glm::mat4 modelViewTransform = glm::mat4(1.0f);

GLuint pointCloudShader = 0;


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
	glutSetOption(GLUT_MULTISAMPLE, 4);

	// Setup window
	int windowWidth = 1920;
	int windowHeight = 1080;
	glutInitWindowSize(windowWidth, windowHeight);
	glutInitWindowPosition(64, 64);
	glutCreateWindow("Terrain");
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
	//cout << "Extensions supported...\n\n";

	//const char* glExtensionString = (const char*)glGetString(GL_EXTENSIONS);

	//char* strEnd = (char*)glExtensionString + strlen(glExtensionString);
	//char* sptr = (char*)glExtensionString;

	//while (sptr < strEnd) {

	//	int slen = (int)strcspn(sptr, " ");
	//	printf("%.*s\n", slen, sptr);
	//	sptr += slen + 1;
	//}

	// Initialise OpenGL...

	wglSwapIntervalEXT(0);

	//glEnable(GL_CULL_FACE);
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
	mainCamera = new ArcballCamera(0.0f, 0.0f, 5.0f, 55.0f, viewportAspect, 0.1f, 10.0f);

	principleAxes = new PrincipleAxesModel();

	//texturedQuad = new TexturedQuadModel("..\\shared_resources\\Textures\\bumblebee.png");

	pExtrude = new PointExtrudeModel();

	gField = new GrassField(1);

	

	sphereModel = new AIScene("..\\shared_resources\\Models\\gsphere.obj", aiProcess_GenNormals | aiProcess_CalcTangentSpace | aiProcess_Triangulate | aiProcess_JoinIdenticalVertices | aiProcess_SortByPType);
	sphereTexture = fiLoadTexture("..\\shared_resources\\Textures\\land_ocean_ice_cloud_2048.bmp", TextureProperties(false));	
	//sphereShader = setupShaders(string("shaders\\basic_texture.vs.txt"), string(""), string("shaders\\basic_texture.fs.txt"));
	sphereShader = setupShaders(string("shaders\\mesh_phong.vs.txt"), string(""), string("shaders\\mesh_phong.fs.txt"));
	pointCloudShader = setupShaders(string("shaders\\point_cloud.vs.txt"), string("shaders\\point_cloud.gs.txt"), string("shaders\\point_cloud.fs.txt"));

	bTerrain = new BasicTerrain(256);

	//old patch terrain function
	//pTerrain = new PatchTerrain(128);

	pTerrain = new PatchTerrain(256, glm::vec2(0.0f, 0.0f), glm::vec2(3.0f, 3.0f), 4);

	//tessTriangle = new BasicTessTriangle();

	tessTriangle = new BasicTessTri();
	tessQuad = new BasicTessQuad();
	tessIsoline = new BasicTessIsoline();
	bezierCurve = new BezierCurveTess();
	crSpline = new CatmulRomSpline();
	bezierSurface = new BezierSurface();
	bsplineSurface = new BSplineSurface(6, 6);

	GLuint textUnit;

	particleSystem = new ParticleSystemGPU(500);

	cout << "sizeof particle = " << sizeof(BasicParticleGPU) << endl;
	cout << offsetof(BasicParticleGPU, pos) << endl;
	cout << offsetof(BasicParticleGPU, velocity) << endl;
	cout << offsetof(BasicParticleGPU, age) << endl;
	cout << offsetof(BasicParticleGPU, isaGenerator) << endl;

	perlinNoiseTexture = new ProceduralTexture();
	//generate a new texture
	GLuint procTexture = perlinNoiseTexture->generateTexture(256, 256);

	texturedQuad = new TexturedQuadModel(procTexture); // assign to the old textured quad model - we can use the texture as we have done with textures loaded from disk.

	skybox = new SkyBox(string("..\\shared_resources\\Textures\\cubemap_snow\\"), string("snow"), string(".png"));

	//glGetIntegeri_v(GL_MAX_TEXTURE_IMAGE_UNITS, textUnit, );

	/*
	// Setup and start the main clock
	mainClock = new GUClock();
	*/


	//terrain
	//1 buffer - height
	//constructor  - create array to store points
	//for loop - populate with co-ordinates, y = 0
	//

}

// Main scene update function (called by FreeGLUT's main event loop every frame) 
void update(void) {

	mainClock->tick();

	// Redraw the screen
	display();

	//if (particleSystem)
	//particleSystem->update(float(mainClock->gameTimeDelta()));

	// Update the window title to show current frames-per-second and seconds-per-frame data
	char timingString[256];
	sprintf_s(timingString, 256, "SJ_Terrain: Average fps: %.0f; Average spf: %f", mainClock->averageFPS(), mainClock->averageSPF() / 1000.0f);
	glutSetWindowTitle(timingString);
}

void display(void) {

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Set viewport to the client area of the current window
	glViewport(0, 0, glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT));

	// Get view-projection transform as a GUMatrix4
	glm::mat4 T = mainCamera->projectionTransform() * mainCamera->viewTransform();

	if (principleAxes)
		principleAxes->render(T);

	if (texturedQuad)
		texturedQuad->render(mainCamera);

	//if (pExtrude)
	//	pExtrude->render(T);

	if (pTerrain)
		pTerrain->render(mainCamera);

	//if (sphereModel)
	//	sphereModel->renderInstanced(100);

	//*****Tesselation and Domain Co-ords*******

	//if (tessTriangle)
	//	tessTriangle->render(T);

	//if (tessQuad)
	//	tessQuad->render(T);

	//if (tessIsoline)
	//	tessIsoline->render(T);

	//if (bezierCurve)
	//	bezierCurve->render(T);

	//if (crSpline)
	//	crSpline->render(T);

	//if (bezierSurface)
	//	bezierSurface->render(T);

	//if (bsplineSurface)
	//	bsplineSurface->render(T);
	
	//if (tessTriangle)
	//	tessTriangle->render(T);

	/********          Particles                  *******/
	//if (particleSystem)
	//	particleSystem->render(mainCamera, float(mainClock->gameTimeDelta()));


	/*glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);*/
	//glDepthMask(GL_FALSE);

	// Render sphere model
	//glPointSize(2.0f);
	//glBindTexture(GL_TEXTURE_2D, sphereTexture);

	//auto shader = pointCloudShader;
	////auto shader = sphereShader;

	//static const GLint mvpLocation = glGetUniformLocation(shader, "mvpMatrix");
	//glUseProgram(shader);
	//modelViewTransform = T * sphereTransform;
	//glUniformMatrix4fv(mvpLocation, 1, GL_FALSE, (const GLfloat*)&modelViewTransform);
	//sphereModel->render();

	//glDepthMask(GL_TRUE);
	//glDisable(GL_BLEND);

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

	// Toggle fullscreen (This does not adjust the display mode however!)
	if (key == 'f')
		glutFullScreenToggle();
}


#pragma endregion

