#pragma once

#include <src/GLProgramObject.hpp>
#include <src/GLShaderFactory.hpp>

class BasicTessIsoline {

private:

	GLuint					vao;

	GLuint					positionBuffer;
	GLuint					colourBuffer;
	GLuint					texCoordBuffer;
	GLuint					indexBuffer;

	GLProgramObject			shaderProg;


public:

	BasicTessIsoline();

	~BasicTessIsoline();

	void render(const glm::mat4& T);
};
