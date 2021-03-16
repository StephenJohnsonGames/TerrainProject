
#include "pch.h"
#include "BSplineSurface.hpp"


using namespace std;

/*
Example...

+--+--+--+--+--+
|  |  |  |  |  |
+--+--+--+--+--+
|  |  |  |  |  |
+--+--+--+--+--+
|  |  |  |  |  |
+--+--+--+--+--+
|  |  |  |  |  |
+--+--+--+--+--+
|  |  |  |  |  |
+--+--+--+--+--+

*/

BSplineSurface::BSplineSurface(GLuint w, GLuint h) {

	// Setup arrays
	//

	// num patches across (Pw) and down (Ph)
	GLuint Pw = w - 3;
	GLuint Ph = h - 3;

	numPoints = w * h;
	numIndices = (Pw * Ph) << 4;
	//numEdgeIndices = (numPoints << 1) + w + h;

	glm::vec4* positionArray = (glm::vec4*)malloc(numPoints * sizeof(glm::vec4));
	glm::vec4* colourArray = (glm::vec4*)malloc(numPoints * sizeof(glm::vec4));
	glm::vec2* texCoordArray = (glm::vec2*)malloc(numPoints * sizeof(glm::vec2));
	GLuint* indexArray = (GLuint*)malloc(numIndices * sizeof(GLuint));
	//ctrlEdgeIndices = (DWORD*)malloc(numEdgeIndices * sizeof(DWORD));

	// Setup control point data
	for (GLuint i=0, y = 0; y < h; ++y) {

		for (UINT x = 0; x < w; ++x, ++i) {

			const float yScale = 0.9f;

			positionArray[i] = glm::vec4(float(x) - float(w >> 1), sinf(float(x)) * cosf(float(h - 1 - y)) * x * yScale, float(h - 1 - y) - float(h >> 1), 1.0f);
			
			colourArray[i] = glm::vec4(0.8f, 0.8f, 0.8f, 1.0f);

			texCoordArray[i] = glm::vec2(float(x) / float(w - 1), float(y) / float(h - 1));

		}
	}

	// Setup cubic patch indices
	GLuint *iptr = indexArray;
	for (UINT j = 0; j < Ph; ++j) {

		for (UINT i = 0; i < Pw; ++i, iptr += 16) {

			GLuint I[] = {

				j*w + i, j*w + (i + 1), j*w + (i + 2), j*w + (i + 3),
				(j + 1)*w + i, (j + 1)*w + (i + 1), (j + 1)*w + (i + 2), (j + 1)*w + (i + 3),
				(j + 2)*w + i, (j + 2)*w + (i + 1), (j + 2)*w + (i + 2), (j + 2)*w + (i + 3),
				(j + 3)*w + i, (j + 3)*w + (i + 1), (j + 3)*w + (i + 2), (j + 3)*w + (i + 3)
			};

			memcpy_s(iptr, sizeof(GLuint) << 4, I, sizeof(GLuint) << 4);
		}
	}

	// Setup horizontal control edge indices
	/*iptr = ctrlEdgeIndices;
	for (UINT j = 0; j < h; ++j) {

		for (UINT i = 0; i < w; ++i, ++iptr)
			*iptr = j * w + i;

		*iptr = -1;
		++iptr;
	}

	// Setup vertical control edge indices
	for (UINT i = 0; i < w; ++i) {

		for (UINT j = 0; j < h; ++j, ++iptr)
			*iptr = j * w + i;

		*iptr = -1;
		++iptr;
	}*/


	glCreateVertexArrays(1, &vao);

	// Setup position VBO
	glCreateBuffers(1, &positionBuffer);
	glNamedBufferStorage(positionBuffer, numPoints * sizeof(glm::vec4), positionArray, 0);
	glVertexArrayVertexBuffer(vao, 0, positionBuffer, 0, sizeof(glm::vec4));
	glVertexArrayAttribBinding(vao, 0, 0);
	glVertexArrayAttribFormat(vao, 0, 4, GL_FLOAT, GL_FALSE, 0);
	glEnableVertexArrayAttrib(vao, 0);

	// Setup colour VBO
	glCreateBuffers(1, &colourBuffer);
	glNamedBufferStorage(colourBuffer, numPoints * sizeof(glm::vec4), colourArray, 0);
	glVertexArrayVertexBuffer(vao, 1, colourBuffer, 0, sizeof(glm::vec4));
	glVertexArrayAttribBinding(vao, 1, 1);
	glVertexArrayAttribFormat(vao, 1, 4, GL_FLOAT, GL_FALSE, 0);
	glEnableVertexArrayAttrib(vao, 1);

	// Setup texture coord VBO
	glCreateBuffers(1, &texCoordBuffer);
	glNamedBufferStorage(texCoordBuffer, numPoints * sizeof(glm::vec2), texCoordArray, 0);
	glVertexArrayVertexBuffer(vao, 2, texCoordBuffer, 0, sizeof(glm::vec2));
	glVertexArrayAttribBinding(vao, 2, 2);
	glVertexArrayAttribFormat(vao, 2, 2, GL_FLOAT, GL_FALSE, 0);
	glEnableVertexArrayAttrib(vao, 2);

	// Setup index buffer
	glCreateBuffers(1, &indexBuffer);
	glNamedBufferStorage(indexBuffer, numIndices * sizeof(GLuint), indexArray, 0);
	glVertexArrayElementBuffer(vao, indexBuffer);

	shaderProg = GLShaderFactory::CreateProgramObject(
		GLShaderFactory::CreateShader(GL_VERTEX_SHADER, "shaders\\Tessellation\\basic_tess.vs.txt"),
		GLShaderFactory::CreateShader(GL_TESS_CONTROL_SHADER, "shaders\\Tessellation\\bspline_surface.tcs.txt"),
		GLShaderFactory::CreateShader(GL_TESS_EVALUATION_SHADER, "shaders\\Tessellation\\bspline_surface.tes.txt"),
		GLShaderFactory::CreateShader(GL_FRAGMENT_SHADER, "shaders\\Tessellation\\basic_tess.fs.txt")
	);
}


BSplineSurface::~BSplineSurface() {

	glBindVertexArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	glDeleteBuffers(1, &positionBuffer);
	glDeleteBuffers(1, &colourBuffer);
	glDeleteBuffers(1, &texCoordBuffer);
	glDeleteBuffers(1, &indexBuffer);

	glDeleteVertexArrays(1, &vao);
}


void BSplineSurface::render(const glm::mat4& T) {

	GLuint shader = shaderProg->useProgramObject();

	glUniformMatrix4fv(glGetUniformLocation(shader, "mvpMatrix"), 1, GL_FALSE, (const GLfloat*)&(T));

	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	glBindVertexArray(vao);
	glPatchParameteri(GL_PATCH_VERTICES, 16); // 16 vertices per patch forming a 4x4 vertex patch
	glDrawElements(GL_PATCHES, numIndices, GL_UNSIGNED_INT, (const GLvoid*)0);
}
