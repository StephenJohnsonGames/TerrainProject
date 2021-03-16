
#include "pch.h"
#include "BezierSurface.hpp"


using namespace std;


// Example data for Catmull-Rom Spline

static glm::vec4 positionArray[] = {

	glm::vec4(0.0f, 0.0f, 3.0f, 1.0f),
	glm::vec4(1.0f, 0.5f, 3.0f, 1.0f),
	glm::vec4(2.0f, 0.5f, 3.0f, 1.0f),
	glm::vec4(3.0f, 0.0f, 3.0f, 1.0f),

	glm::vec4(0.0f, 0.0f, 2.0f, 1.0f),
	glm::vec4(1.0f, 1.0f, 2.0f, 1.0f),
	glm::vec4(2.0f, 1.0f, 2.0f, 1.0f),
	glm::vec4(3.0f, 0.0f, 2.0f, 1.0f),

	glm::vec4(0.0f, 0.0f, 1.0f, 1.0f),
	glm::vec4(1.0f, 1.0f, 1.0f, 1.0f),
	glm::vec4(2.0f, 1.0f, 1.0f, 1.0f),
	glm::vec4(3.0f, 0.0f, 1.0f, 1.0f),

	glm::vec4(0.0f, 0.0f, 0.0f, 1.0f),
	glm::vec4(1.0f, 0.5f, 0.0f, 1.0f),
	glm::vec4(2.0f, 0.5f, 0.0f, 1.0f),
	glm::vec4(3.0f, 0.0f, 0.0f, 1.0f)
};

static glm::vec4 colourArray[] = {

	glm::vec4(1.0f, 1.0f, 0.0f, 1.0f),
	glm::vec4(1.0f, 1.0f, 0.0f, 1.0f),
	glm::vec4(1.0f, 1.0f, 0.0f, 1.0f),
	glm::vec4(1.0f, 1.0f, 0.0f, 1.0f),

	glm::vec4(0.0f, 0.0f, 1.0f, 1.0f),
	glm::vec4(0.0f, 0.0f, 1.0f, 1.0f),
	glm::vec4(0.0f, 0.0f, 1.0f, 1.0f),
	glm::vec4(0.0f, 0.0f, 1.0f, 1.0f),

	glm::vec4(0.0f, 1.0f, 0.0f, 1.0f),
	glm::vec4(0.0f, 1.0f, 0.0f, 1.0f),
	glm::vec4(0.0f, 1.0f, 0.0f, 1.0f),
	glm::vec4(0.0f, 1.0f, 0.0f, 1.0f),

	glm::vec4(1.0f, 0.0f, 0.0f, 1.0f),
	glm::vec4(1.0f, 0.0f, 0.0f, 1.0f),
	glm::vec4(1.0f, 0.0f, 0.0f, 1.0f),
	glm::vec4(1.0f, 0.0f, 0.0f, 1.0f)
};

static glm::vec2 texCoordArray[] = {

	glm::vec2(0.0f, 0.0f),
	glm::vec2(0.33f, 0.0f),
	glm::vec2(0.66f, 0.0f),
	glm::vec2(1.0f, 0.0f),

	glm::vec2(0.0f, 0.33f),
	glm::vec2(0.33f, 0.33f),
	glm::vec2(0.66f, 0.33f),
	glm::vec2(1.0f, 0.33f),

	glm::vec2(0.0f, 0.66f),
	glm::vec2(0.33f, 0.66f),
	glm::vec2(0.66f, 0.66f),
	glm::vec2(1.0f, 0.66f),

	glm::vec2(0.0f, 1.0f),
	glm::vec2(0.33f, 1.0f),
	glm::vec2(0.66f, 1.0f),
	glm::vec2(1.0f, 1.0f)
};

// Model indices for each Catmull-Rom Spline section.  Each curve section Qi(t) is modelled with control points (Pi-1, Pi, Pi+1, Pi+2).  Pi-1 and Pi+2 are used to calculate the tangent vectors at Pi and Pi+1

static unsigned int indexArray[] = {

	0,	1,	2,	3,
	4,	5,	6,	7,
	8,	9,	10,	11,
	12,	13,	14,	15
};



BezierSurface::BezierSurface() {

	glCreateVertexArrays(1, &vao);

	// Setup position VBO
	glCreateBuffers(1, &positionBuffer);
	glNamedBufferStorage(positionBuffer, sizeof(positionArray), positionArray, 0);
	glVertexArrayVertexBuffer(vao, 0, positionBuffer, 0, sizeof(glm::vec4));
	glVertexArrayAttribBinding(vao, 0, 0);
	glVertexArrayAttribFormat(vao, 0, 4, GL_FLOAT, GL_FALSE, 0);
	glEnableVertexArrayAttrib(vao, 0);

	// Setup colour VBO
	glCreateBuffers(1, &colourBuffer);
	glNamedBufferStorage(colourBuffer, sizeof(colourArray), colourArray, 0);
	glVertexArrayVertexBuffer(vao, 1, colourBuffer, 0, sizeof(glm::vec4));
	glVertexArrayAttribBinding(vao, 1, 1);
	glVertexArrayAttribFormat(vao, 1, 4, GL_FLOAT, GL_FALSE, 0);
	glEnableVertexArrayAttrib(vao, 1);

	// Setup texture coord VBO
	glCreateBuffers(1, &texCoordBuffer);
	glNamedBufferStorage(texCoordBuffer, sizeof(texCoordArray), texCoordArray, 0);
	glVertexArrayVertexBuffer(vao, 2, texCoordBuffer, 0, sizeof(glm::vec2));
	glVertexArrayAttribBinding(vao, 2, 2);
	glVertexArrayAttribFormat(vao, 2, 2, GL_FLOAT, GL_FALSE, 0);
	glEnableVertexArrayAttrib(vao, 2);

	// Setup index buffer
	glCreateBuffers(1, &indexBuffer);
	glNamedBufferStorage(indexBuffer, sizeof(indexArray), indexArray, 0);
	glVertexArrayElementBuffer(vao, indexBuffer);

	shaderProg = GLShaderFactory::CreateProgramObject(
		GLShaderFactory::CreateShader(GL_VERTEX_SHADER, "shaders\\Tessellation\\basic_tess.vs.txt"),
		GLShaderFactory::CreateShader(GL_TESS_CONTROL_SHADER, "shaders\\Tessellation\\bezier_surface.tcs.txt"),
		GLShaderFactory::CreateShader(GL_TESS_EVALUATION_SHADER, "shaders\\Tessellation\\bezier_surface.tes.txt"),
		GLShaderFactory::CreateShader(GL_FRAGMENT_SHADER, "shaders\\Tessellation\\basic_tess.fs.txt")
	);
}


BezierSurface::~BezierSurface() {

	glBindVertexArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	glDeleteBuffers(1, &positionBuffer);
	glDeleteBuffers(1, &colourBuffer);
	glDeleteBuffers(1, &texCoordBuffer);
	glDeleteBuffers(1, &indexBuffer);

	glDeleteVertexArrays(1, &vao);
}


void BezierSurface::render(const glm::mat4& T) {

	GLuint shader = shaderProg->useProgramObject();

	glUniformMatrix4fv(glGetUniformLocation(shader, "mvpMatrix"), 1, GL_FALSE, (const GLfloat*)&(T));

	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	glBindVertexArray(vao);
	glPatchParameteri(GL_PATCH_VERTICES, 16); // 16 vertices per patch forming a 4x4 vertex patch
	glDrawElements(GL_PATCHES, 16, GL_UNSIGNED_INT, (const GLvoid*)0);
}
