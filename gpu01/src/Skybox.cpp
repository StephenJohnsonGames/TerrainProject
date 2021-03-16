
#include <pch.h>
#include "Skybox.hpp"

using namespace std;


#pragma region Skybox model

// Packed vertex buffer for skybox model
static float positionArray[] = {

	-5.0f, -5.0f, -5.0f, 1.0f,
	5.0f, -5.0f, -5.0f, 1.0f,
	5.0f, 5.0f, -5.0f, 1.0f,
	-5.0f, 5.0f, -5.0f, 1.0f,
	
	-5.0f, -5.0f, 5.0f, 1.0f,
	-5.0f, 5.0f, 5.0f, 1.0f,
	5.0f, 5.0f, 5.0f, 1.0f,
	5.0f, -5.0f, 5.0f, 1.0f
};


static float normalArray[] = {

	-1.0f, -1.0f, -1.0f,
	1.0f, -1.0f, -1.0f,
	1.0f, 1.0f, -1.0f,
	-1.0f, 1.0f, -1.0f,
	
	-1.0f, -1.0f, 1.0f,
	-1.0f, 1.0f, 1.0f,
	1.0f, 1.0f, 1.0f,
	1.0f, -1.0f, 1.0f
};

static GLuint indexArray[] = {

	1, 2, 0, 3, 4, 5, 7, 6, 1, 2, GLuint(-1), 3, 2, 5, 6, GLuint(-1), 1, 0, 7, 4
};

#pragma endregion


SkyBox::SkyBox(const string& cubemapPath, const string& cubemapName, const string& cubemapExtension) {

	// Load shader
	shader = setupShaders(string("shaders\\Skybox\\skybox.vs.txt"), string(""), string("shaders\\Skybox\\skybox.fs.txt"));

	// Setup VAO
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	// Setup VBO for vertex position data
	glGenBuffers(1, &positionBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, positionBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(positionArray), positionArray, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, (const GLvoid*)0);

	// Setup VBO for normal vector data
	glGenBuffers(1, &normalBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, normalBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(normalArray), normalArray, GL_STATIC_DRAW);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_TRUE, 0, (const GLvoid*)0);

	glGenBuffers(1, &indexBuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indexArray), indexArray, GL_STATIC_DRAW);

	// Enable vertex buffers
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(2);

	// Unbind VAO
	glBindVertexArray(0);


	//
	// Load cubemap texture
	//

#if 1

	// This version uses GL_MIRRORED_REPEAT to help eliminate seam edges in the cube map and skybox rendering

	texture = loadCubeMapTexture(
		cubemapPath, cubemapName, cubemapExtension,
		GL_SRGB8_ALPHA8,
		GL_LINEAR,
		GL_LINEAR,
		4.0f,
		GL_MIRRORED_REPEAT,
		GL_MIRRORED_REPEAT,
		GL_MIRRORED_REPEAT,
		true,
		true);

#else

	// This version uses GL_REPEAT which introduces cubemap sampling artefacts at the corners and edges of the skybox

	texture = loadCubeMapTexture(
		cubemapPath, cubemapName, cubemapExtension,
		GL_SRGB8_ALPHA8,
		GL_LINEAR,
		GL_LINEAR,
		4.0f,
		GL_REPEAT,
		GL_REPEAT,
		GL_REPEAT,
		false,
		true);

#endif

}


SkyBox::~SkyBox() {

	// Unbind VAO
	glBindVertexArray(0);

	// Unbind VBOs
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	// Delete GL resources
	glDeleteBuffers(1, &positionBuffer);
	glDeleteBuffers(1, &normalBuffer);
	glDeleteBuffers(1, &indexBuffer);

	glDeleteVertexArrays(1, &vao);
	
	glDeleteShader(shader);
	
	glDeleteTextures(1, &texture);
}


GLuint SkyBox::skyboxTexture() {

	return texture;
}


void SkyBox::setSkyboxTexture(const GLuint newTexture) {

	if (newTexture == 0)
		return;

	if (texture)
		glDeleteTextures(1, &texture);

	texture = newTexture;
}


void SkyBox::render(cst::ArcballCamera* camera) {

	static GLint mvpLocation = glGetUniformLocation(shader, "mvpMatrix");
	glUseProgram(shader);
	glUniformMatrix4fv(mvpLocation, 1, GL_FALSE, (const GLfloat*) & (camera));

	glm::mat4 viewTransform = camera->viewTransform();
	glm::mat4 projectionTransform = camera->projectionTransform();

	static GLint viewLocation = glGetUniformLocation(shader, "viewMatrix");
	glUseProgram(shader);
	glUniformMatrix4fv(viewLocation, 1, GL_FALSE, (const GLfloat*)& viewTransform);

	static GLint projLocation = glGetUniformLocation(shader, "projMatrix");
	glUseProgram(shader);
	glUniformMatrix4fv(projLocation, 1, GL_FALSE, (const GLfloat*)& projectionTransform);

	float nearPlaneDist = camera->getViewFrustum().nearPlaneDistance();
	float farPlaneDist = camera->getViewFrustum().farPlaneDistance();

	static GLint nearLocation = glGetUniformLocation(shader, "near");
	glUseProgram(shader);
	glUniform1f(nearLocation, nearPlaneDist);

	static GLint farLocation = glGetUniformLocation(shader, "far");
	glUseProgram(shader);
	glUniform1f(farLocation, farPlaneDist);

	//glUseProgram(shader);
	//glUniformMatrix4fv(glGetUniformLocation(shader, "mvpMatrix"), 1, GL_FALSE, (const GLfloat*)&(camera));

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, texture);

	glBindVertexArray(vao);

	glEnable(GL_PRIMITIVE_RESTART);
	glPrimitiveRestartIndex(GLuint(-1));

	glDrawElements(GL_TRIANGLE_STRIP, 20, GL_UNSIGNED_INT, (const void*)0);

	glDisable(GL_PRIMITIVE_RESTART);

	// Unbind VAO once drawing complete
	glBindVertexArray(0);
}
