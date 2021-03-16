#pragma once

#include <src/GLProgramObject.hpp>
#include <src/GLShaderFactory.hpp>

class CatmulRomSpline {

private:

	GLuint					vao;

	GLuint					positionBuffer;
	GLuint					colourBuffer;
	GLuint					texCoordBuffer;
	GLuint					indexBuffer;

	GLProgramObject			shaderProg;


public:

	CatmulRomSpline();

	~CatmulRomSpline();

	void render(const glm::mat4& T);
};

