#pragma once

class AIScene;

namespace cst {

	class ArcballCamera;
};

class ReflectiveMesh {

	AIScene*		model;
	GLuint			envTexture;

	GLuint			shader;

public:

	ReflectiveMesh(AIScene* model, GLuint envTexture);

	void render(cst::ArcballCamera* camera);
};