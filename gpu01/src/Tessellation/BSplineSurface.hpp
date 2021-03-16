#pragma once

#include <src/GLProgramObject.hpp>
#include <src/GLShaderFactory.hpp>

class BSplineSurface {

private:

	GLuint					vao;

	GLuint					numPoints;
	GLuint					numIndices;

	GLuint					positionBuffer;
	GLuint					colourBuffer;
	GLuint					texCoordBuffer;
	GLuint					indexBuffer;

	GLProgramObject			shaderProg;


public:

	BSplineSurface(GLuint w, GLuint h);

	~BSplineSurface();

	void render(const glm::mat4& T);
};

