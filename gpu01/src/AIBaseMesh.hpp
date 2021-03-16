#pragma once

#include "GLShaderFactory.hpp"

class AIScene;

namespace cst {
	class ArcballCamera;
};

class AIBaseMesh {

	AIScene*			model;

	GLuint				texture;

	GLProgramObject		shader;

	// Private API

	void setupModel(const char* modelFilename);
	void setupShader();

public:

	AIBaseMesh(const char* modelFilename, const char* textureFilename);
	AIBaseMesh(const char* modelFilename, GLuint textureObjectID);

	void render(cst::ArcballCamera* camera);
	void renderInstanced(GLsizei numInstances);
};