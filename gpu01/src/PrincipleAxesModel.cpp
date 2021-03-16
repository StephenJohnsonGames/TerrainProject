
#include "pch.h"
#include "PrincipleAxesModel.hpp"


using namespace std;


// Example data for principle axes

// Packed vertex buffer for principle axes model
static float paPositionArray[] = {

	// x
	0.0f, 0.0f, 0.0f, 1.0f,
	1.0f, 0.0f, 0.0f, 1.0f,
	1.025f, 0.025f, 0.0, 1.0f,
	1.075f, 0.075f, 0.0, 1.0f,
	1.025f, 0.075f, 0.0, 1.0f,
	1.075f, 0.025f, 0.0, 1.0f,

	// y
	0.0, 0.0, 0.0, 1.0f,
	0.0, 1.0, 0.0, 1.0f,
	-0.075f, 1.075f, 0.0, 1.0f,
	-0.05f, 1.05f, 0.0, 1.0f,
	-0.025f, 1.075f, 0.0, 1.0f,
	-0.075f, 1.025f, 0.0, 1.0f,

	// z
	0.0, 0.0, 0.0, 1.0f,
	0.0, 0.0, 1.0, 1.0f,
	0.025f, 0.075f, 1.0, 1.0f,
	0.075f, 0.075f, 1.0, 1.0f,
	0.025f, 0.025f, 1.0, 1.0f,
	0.075f, 0.025f, 1.0, 1.0f
};

// Packed colour buffer for principle axes model
static float paColourArray[] = {

	// x
	1.0f, 0.0f, 0.0f, 1.0f,
	1.0f, 0.0f, 0.0f, 1.0f,
	1.0f, 0.0f, 0.0f, 1.0f,
	1.0f, 0.0f, 0.0f, 1.0f,
	1.0f, 0.0f, 0.0f, 1.0f,
	1.0f, 0.0f, 0.0f, 1.0f,

	// y
	0.0f, 1.0f, 0.0f, 1.0f,
	0.0f, 1.0f, 0.0f, 1.0f,
	0.0f, 1.0f, 0.0f, 1.0f,
	0.0f, 1.0f, 0.0f, 1.0f,
	0.0f, 1.0f, 0.0f, 1.0f,
	0.0f, 1.0f, 0.0f, 1.0f,

	// z
	0.28f, 0.5f, 0.9f, 1.0f,
	0.28f, 0.5f, 0.9f, 1.0f,
	0.28f, 0.5f, 0.9f, 1.0f,
	0.28f, 0.5f, 0.9f, 1.0f,
	0.28f, 0.5f, 0.9f, 1.0f,
	0.28f, 0.5f, 0.9f, 1.0f
};


// Line list topology to render principle axes
static unsigned int paIndexArray[] = {

	0, 1, 2, 3, 4, 5,					// x
	6, 7, 8, 9, 10, 11,					// y
	12, 13, 14, 15, 15, 16, 16, 17		// z
};



PrincipleAxesModel::PrincipleAxesModel() {

	// setup shader for principle axes
	paShader = setupShaders(string("Shaders\\basic_shader.vs.txt"), string(""), string("Shaders\\basic_shader.fs.txt"));

	// setup VAO for principle axes object
	glCreateVertexArrays(1, &paVertexArrayObj);

	// setup vbo for position attribute
	glCreateBuffers(1, &paVertexBuffer);
	glNamedBufferStorage(paVertexBuffer, sizeof(paPositionArray), paPositionArray, 0);

	glVertexArrayVertexBuffer(paVertexArrayObj, 0, paVertexBuffer, 0, sizeof(float) << 2);
	glVertexArrayAttribBinding(paVertexArrayObj, 0, 0);
	glVertexArrayAttribFormat(paVertexArrayObj, 0, 4, GL_FLOAT, GL_FALSE, 0);
	glEnableVertexArrayAttrib(paVertexArrayObj, 0);

	// Setup for colour attribute
	glCreateBuffers(1, &paColourBuffer);
	glNamedBufferStorage(paColourBuffer, sizeof(paColourArray), paColourArray, 0);

	glVertexArrayVertexBuffer(paVertexArrayObj, 1, paColourBuffer, 0, sizeof(float) << 2);
	glVertexArrayAttribBinding(paVertexArrayObj, 1, 1);
	glVertexArrayAttribFormat(paVertexArrayObj, 1, 4, GL_FLOAT, GL_FALSE, 0);
	glEnableVertexArrayAttrib(paVertexArrayObj, 1);

	// Setup index buffer
	glCreateBuffers(1, &paIndexBuffer);
	glNamedBufferStorage(paIndexBuffer, sizeof(paIndexArray), paIndexArray, 0);
	glVertexArrayElementBuffer(paVertexArrayObj, paIndexBuffer);
}


PrincipleAxesModel::~PrincipleAxesModel() {

	glBindVertexArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	glDeleteBuffers(1, &paVertexBuffer);
	glDeleteBuffers(1, &paColourBuffer);
	glDeleteBuffers(1, &paIndexBuffer);

	glDeleteVertexArrays(1, &paVertexArrayObj);

	glDeleteShader(paShader);
}


void PrincipleAxesModel::render(const glm::mat4& T) {

	static GLint mvpLocation = glGetUniformLocation(paShader, "mvpMatrix");

	glUseProgram(paShader);
	glUniformMatrix4fv(mvpLocation, 1, GL_FALSE, (const GLfloat*)&(T));

	glBindVertexArray(paVertexArrayObj);
	glDrawElements(GL_LINES, 20, GL_UNSIGNED_INT, (const GLvoid*)0);
}
