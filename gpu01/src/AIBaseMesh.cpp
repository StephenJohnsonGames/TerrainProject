#include <pch.h>
#include "AIBaseMesh.hpp"
#include "AIScene.hpp"
#include "ArcballCamera.hpp"

using namespace std;
using namespace cst;

// Private API

void AIBaseMesh::setupModel(const char* modelFilename) {

	model = new AIScene(modelFilename, aiProcess_GenNormals | aiProcess_CalcTangentSpace | aiProcess_Triangulate | aiProcess_JoinIdenticalVertices | aiProcess_SortByPType);
}

void AIBaseMesh::setupShader() {

	shader = GLShaderFactory::CreateProgramObject(
		GLShaderFactory::CreateShader(GL_VERTEX_SHADER, "shaders\\base_mesh.vs.txt"),
		GLShaderFactory::CreateShader(GL_FRAGMENT_SHADER, "shaders\\base_mesh.fs.txt")
	);
}


AIBaseMesh::AIBaseMesh(const char* modelFilename, const char* textureFilename) {

	setupModel(modelFilename);
	setupShader();

	TextureProperties texProperties = TextureProperties();
	texture = fiLoadTexture(textureFilename, texProperties);
}

AIBaseMesh::AIBaseMesh(const char* modelFilename, GLuint textureObjectID) {

	setupModel(modelFilename);
	setupShader();

	texture = textureObjectID;
}

void AIBaseMesh::render(ArcballCamera* camera) {

	// calculate camera location in world coords

	glm::vec4 cameraPos = camera->getPosition();
	glm::mat4 modelTransform = glm::translate(glm::mat4(1.0f), glm::vec3(1.0f, 0.0f, 0.5f)) * glm::scale(glm::mat4(1.0f), glm::vec3(0.001f, 0.001f, 0.001f));
	glm::mat4 invTranspose = glm::inverseTranspose(modelTransform);
	glm::mat4 mvp = camera->projectionTransform() * camera->viewTransform() * modelTransform;

	GLuint shader = this->shader->useProgramObject();

	// setup uniforms for current frame
	glUniformMatrix4fv(glGetUniformLocation(shader, "modelMatrix"), 1, GL_FALSE, (const GLfloat*)&(modelTransform));
	glUniformMatrix4fv(glGetUniformLocation(shader, "invTransposeModelMatrix"), 1, GL_FALSE, (const GLfloat*)&(invTranspose));
	glUniformMatrix4fv(glGetUniformLocation(shader, "modelViewProjectionMatrix"), 1, GL_FALSE, (const GLfloat*)&(mvp));
	glUniform4f(glGetUniformLocation(shader, "cameraWorldPos"), cameraPos.x, cameraPos.y, cameraPos.z, 1.0f);
	
	// bind texture
	glActiveTexture(0);
	glBindTexture(GL_TEXTURE_2D, texture);

	// render cubemapped sphere
	model->render();

	// restore fixed function rendering ready for next frame
	glUseProgram(0);
}

// This does no shader setup - this is done by the caller.  All this does is initiate the render call
void AIBaseMesh::renderInstanced(GLsizei numInstances)  {

	// bind texture
	glActiveTexture(0);
	glBindTexture(GL_TEXTURE_2D, texture);

	model->renderInstanced(numInstances);
}