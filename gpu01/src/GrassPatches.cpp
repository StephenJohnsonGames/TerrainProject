#include <pch.h>
#include "GrassPatches.hpp"
#include "ArcballCamera.hpp"
#include "PerlinNoise.hpp"

using namespace std;
using namespace cst;

struct alignas(16) GrassInstance {

	alignas(16) glm::vec4					posW;
	alignas(16) glm::vec3					scaleFactor;

	GrassInstance(glm::vec4 posW, glm::vec3 scaleFactor) {

		this->posW = posW;
		this->scaleFactor = scaleFactor;
	}
};

GrassPatches::GrassPatches(GLuint numInstances, const glm::vec2& domainCoord, const glm::vec2& domainExtent, function<float(float, float)> fractalLambda) {

	this->numInstances = numInstances;

	//
	// Setup instance buffer
	//

	glCreateBuffers(1, &instanceBuffer);
	glNamedBufferStorage(instanceBuffer, numInstances * sizeof(GrassInstance), 0, GL_MAP_WRITE_BIT);

	// Map buffer and initialise generator
	auto instanceArray = (GrassInstance*)glMapNamedBuffer(instanceBuffer, GL_WRITE_ONLY);

	random_device rd;
	mt19937 mt(rd());
	auto uDist = uniform_real_distribution<float>(domainCoord.x, domainCoord.x + domainExtent.x);
	auto vDist = uniform_real_distribution<float>(domainCoord.y, domainCoord.y + domainExtent.y);
	auto scaleDist = uniform_real_distribution<float>(0.9f, 1.1f);

	for (GLuint i = 0; i < numInstances; ++i) {

		float u, v, y;

		do {

			u = uDist(mt);
			v = vDist(mt);
			y = fractalLambda(u, v);

		} while (y >= -0.1f);

		instanceArray[i] = GrassInstance(glm::vec4(u, y, v, 1.0f), glm::vec3(scaleDist(mt) * 0.001f, scaleDist(mt) * 0.001f, scaleDist(mt) * 0.001f));
	}
	

	glUnmapNamedBuffer(instanceBuffer);


	//
	// Setup model
	//
	GLuint texture = fiLoadTexture("..\\shared_resources\\Textures\\GrassTexture.png", TextureProperties(false));
	model = new AIBaseMesh("..\\shared_resources\\Models\\Grass.obj", texture);


	//
	// Setup shaders
	//
	shader = GLShaderFactory::CreateProgramObject(
		GLShaderFactory::CreateShader(GL_VERTEX_SHADER, string("shaders\\Instancing\\base_mesh_inst.vs.txt")),
		GLShaderFactory::CreateShader(GL_FRAGMENT_SHADER, string("shaders\\Instancing\\base_mesh_inst.fs.txt"))
	);

}

void GrassPatches::render(ArcballCamera* camera) {

	// calculate camera location in world coords

	glm::mat4 viewTransform = camera->viewTransform();
	glm::mat4 projectionTransform = camera->projectionTransform();
	float nearPlaneDist = camera->getViewFrustum().nearPlaneDistance();
	float farPlaneDist = camera->getViewFrustum().farPlaneDistance();

	GLuint shader = this->shader->useProgramObject();

	static GLint viewLocation = glGetUniformLocation(shader, "viewMatrix");
	static GLint projLocation = glGetUniformLocation(shader, "projMatrix");
	static GLint nearLocation = glGetUniformLocation(shader, "near");
	static GLint farLocation = glGetUniformLocation(shader, "far");

	glUniformMatrix4fv(viewLocation, 1, GL_FALSE, (const GLfloat*)&viewTransform);
	glUniformMatrix4fv(projLocation, 1, GL_FALSE, (const GLfloat*)&projectionTransform);
	glUniform1f(nearLocation, nearPlaneDist);
	glUniform1f(farLocation, farPlaneDist);

	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, instanceBuffer);

	model->renderInstanced(numInstances);
	
	// restore fixed function rendering ready for next frame
	glUseProgram(0);
}