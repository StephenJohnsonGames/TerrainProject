#pragma once

#include "GLShaderFactory.hpp"
#include "AIBaseMesh.hpp"


class PerlinNoise;

namespace cst {
	class ArcballCamera;
};


class GrassPatches {

	GLProgramObject			shader; // For instanced rendering

	GLuint					numInstances;
	GLuint					instanceBuffer; // SSO to store per-instance data

	AIBaseMesh*			model; // base model to instantiate

public:

	GrassPatches(GLuint numInstances, const glm::vec2& domainCoord, const glm::vec2& domainExtent, std::function<float(float, float)> fn);

	void render(cst::ArcballCamera* camera);
};