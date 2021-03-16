#pragma once

#include <src/GLProgramObject.hpp>
#include <src/GLShaderFactory.hpp>

class BezierSurface {

private:

	GLuint					vao;

	GLuint					positionBuffer;
	GLuint					colourBuffer;
	GLuint					texCoordBuffer;
	GLuint					indexBuffer;

	GLProgramObject			shaderProg;


public:

	BezierSurface();

	~BezierSurface();

	void render(const glm::mat4& T);
};
