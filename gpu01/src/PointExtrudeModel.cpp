
#include "pch.h"
#include "PointExtrudeModel.hpp"


using namespace std;


// Example data

// Packed vertex position buffer
static float paPositionArray[] = {

	-1.0f, 0.0f, -1.0f, 1.0f,
	1.0f, 0.0f, -1.0f, 1.0f,
	1.0f, 0.0f, 1.0f, 1.0f,
	-1.0f, 0.0f, 1.0f, 1.0f
};

// Packed colour buffer
static float paColourArray[] = {

	1.0f, 0.0f, 0.0f, 1.0f,
	0.0f, 1.0f, 0.0f, 1.0f,
	0.28f, 0.5f, 0.9f, 1.0f,
	1.0f, 1.0f, 0.0f, 1.0f
};


// Point topology
static unsigned int paIndexArray[] = {

	0, 1, 2, 3
};



PointExtrudeModel::PointExtrudeModel() {

	// setup shader for principle axes
	paShader = setupShaders(string("shaders\\basic_extrude.vs.txt"), string("shaders\\basic_extrude.gs.txt"), string("shaders\\basic_shader.fs.txt"));

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


PointExtrudeModel::~PointExtrudeModel() {

	glBindVertexArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	glDeleteBuffers(1, &paVertexBuffer);
	glDeleteBuffers(1, &paColourBuffer);
	glDeleteBuffers(1, &paIndexBuffer);

	glDeleteVertexArrays(1, &paVertexArrayObj);

	glDeleteShader(paShader);
}


void PointExtrudeModel::render(const glm::mat4& T) {

	static GLint mvpLocation = glGetUniformLocation(paShader, "mvpMatrix");

	glUseProgram(paShader);
	glUniformMatrix4fv(mvpLocation, 1, GL_FALSE, (const GLfloat*)&(T));

	glBindVertexArray(paVertexArrayObj);
	glDrawElements(GL_POINTS, 4, GL_UNSIGNED_INT, (const GLvoid*)0);
}
