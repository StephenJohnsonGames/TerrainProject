#pragma once

#include <src/GLProgramObject.hpp>
#include <src/GLShaderFactory.hpp>

class BasicTessTri {

private:

	GLuint					vao;

	GLuint					positionBuffer;
	GLuint					colourBuffer;
	GLuint					texCoordBuffer;
	GLuint					indexBuffer;

	GLProgramObject			shaderProg;
	

public:

	BasicTessTri();

	~BasicTessTri();

	void render(const glm::mat4& T);
};

