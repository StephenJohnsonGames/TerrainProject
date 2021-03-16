#pragma once

#include <src/GLProgramObject.hpp>
#include <src/GLShaderFactory.hpp>

class BezierCurveTess {

private:

	GLuint					vao;

	GLuint					positionBuffer;
	GLuint					colourBuffer;
	GLuint					texCoordBuffer;
	GLuint					indexBuffer;

	GLProgramObject			shaderProg;


public:

	BezierCurveTess();

	~BezierCurveTess();

	void render(const glm::mat4& T);
};
