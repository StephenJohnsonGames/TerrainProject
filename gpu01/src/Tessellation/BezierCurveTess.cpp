
#include "pch.h"
#include "BezierCurveTess.hpp"


using namespace std;


// Example data for Bezier Curve

static glm::vec4 positionArray[] = {

	glm::vec4(-1.5f, -0.5f, 1.0f, 1.0f),
	glm::vec4(0.0f, 0.0f, 0.0f, 1.0f),
	glm::vec4(1.25f, 1.0f, 0.0f, 1.0f),
	glm::vec4(-2.0f, 3.0f, -2.0f, 1.0f)
};

static glm::vec4 colourArray[] = {

	glm::vec4(1.0f, 0.0f, 0.0f, 1.0f),
	glm::vec4(0.0f, 1.0f, 0.0f, 1.0f),
	glm::vec4(0.0f, 0.0f, 1.0f, 1.0f),
	glm::vec4(1.0f, 1.0f, 0.0f, 1.0f)
};

static glm::vec2 texCoordArray[] = {

	glm::vec2(0.0f, 0.0f),
	glm::vec2(0.25f, 0.0f),
	glm::vec2(0.5f, 0.0f),
	glm::vec2(1.0f, 0.0f)
};

// Simple line4 patch
static unsigned int indexArray[] = {

	0, 1, 2, 3
};



BezierCurveTess::BezierCurveTess() {

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
		GLShaderFactory::CreateShader(GL_TESS_CONTROL_SHADER, "shaders\\Tessellation\\bezier_curve.tcs.txt"),
		GLShaderFactory::CreateShader(GL_TESS_EVALUATION_SHADER, "shaders\\Tessellation\\bezier_curve.tes.txt"),
		GLShaderFactory::CreateShader(GL_FRAGMENT_SHADER, "shaders\\Tessellation\\basic_tess.fs.txt")
	);
}


BezierCurveTess::~BezierCurveTess() {

	glBindVertexArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	glDeleteBuffers(1, &positionBuffer);
	glDeleteBuffers(1, &colourBuffer);
	glDeleteBuffers(1, &texCoordBuffer);
	glDeleteBuffers(1, &indexBuffer);

	glDeleteVertexArrays(1, &vao);
}


void BezierCurveTess::render(const glm::mat4& T) {

	GLuint shader = shaderProg->useProgramObject();

	glUniformMatrix4fv(glGetUniformLocation(shader, "mvpMatrix"), 1, GL_FALSE, (const GLfloat*)&(T));

	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	glBindVertexArray(vao);
	glPatchParameteri(GL_PATCH_VERTICES, 4); // 4 vertices per patch
	glDrawElements(GL_PATCHES, 4, GL_UNSIGNED_INT, (const GLvoid*)0);
}
