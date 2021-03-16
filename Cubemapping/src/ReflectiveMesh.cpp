#include <pch.h>
#include "ReflectiveMesh.hpp"
#include "ArcballCamera.hpp"
#include "AIScene.hpp"

using namespace std;
using namespace cst;


ReflectiveMesh::ReflectiveMesh(AIScene* model, GLuint envTexture) {

	this->model = model;
	this->envTexture = envTexture;

	shader = setupShaders(string("shaders\\Cubemap\\cubemap.vs.txt"), string(""), string("shaders\\Cubemap\\cubemap.fs.txt"));
}

void ReflectiveMesh::render(ArcballCamera* camera) {

	// calculate camera location in world coords

	glm::vec4 cameraPos = camera->getPosition();

	glm::mat4 modelTransform = glm::mat4(1.0f);
	glm::mat4 invTranspose = glm::inverseTranspose(modelTransform);
	glm::mat4 MVP = camera->projectionTransform() * camera->viewTransform() * modelTransform;

	glUseProgram(shader);

	// setup uniforms for current frame
	glUniformMatrix4fv(glGetUniformLocation(shader, "modelMatrix"), 1, GL_FALSE, (const GLfloat*)&(modelTransform));
	glUniformMatrix4fv(glGetUniformLocation(shader, "invTransposeModelMatrix"), 1, GL_FALSE, (const GLfloat*)&(invTranspose));
	glUniformMatrix4fv(glGetUniformLocation(shader, "modelViewProjectionMatrix"), 1, GL_FALSE, (const GLfloat*)&(MVP));
	glUniform4f(glGetUniformLocation(shader, "cameraWorldPos"), cameraPos.x, cameraPos.y, cameraPos.z, 1.0f);
	glUniform1i(glGetUniformLocation(shader, "cubemap"), 0);

	// bind cubemap texture
	glActiveTexture(0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, envTexture);

	// render cubemapped sphere
	model->render();

	// restore fixed function rendering ready for next frame
	glUseProgram(0);
}