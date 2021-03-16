#include <pch.h>
#include "GrassField.hpp"
#include "RandomEngine.hpp"
#include "GUClock.hpp"

using namespace std;

GrassField::GrassField(const GLuint numPoints) {

	this->numPoints = numPoints;

	GLuint positionArraySize = numPoints * sizeof(glm::vec4);
	GLuint colourArraySize = numPoints * sizeof(glm::vec4);
	GLuint indexArraySize = numPoints * sizeof(GLuint);
	GLuint heightArraySize = numPoints * sizeof(float);

	glm::vec4* positionArray = (glm::vec4*)malloc(positionArraySize);
	glm::vec4* colourArray = (glm::vec4*)malloc(colourArraySize);
	float* heightArray = (float*)malloc(heightArraySize);
	GLuint* indexArray = (GLuint*)malloc(indexArraySize);

	random_device rd;
	mt19937 mt = mt19937(rd());
	uniform_real_distribution<float> positionDist = uniform_real_distribution<float>(-2.0f, 2.0f);

	//populate arrays
	for (GLuint i = 0; i < numPoints; i++) {

		positionArray[i].x = positionDist(mt);
		positionArray[i].y = 0.0f;
		positionArray[i].z = positionDist(mt);
		positionArray[i].w = 1.0f;

		colourArray[i].r = 1.0f;
		colourArray[i].g = 0.0f;
		colourArray[i].b = 1.0f;
		colourArray[i].a = 1.0f;

		heightArray[i] = 2.0f;

		indexArray[i] = i;

	}


	//GPU structures

	
	glCreateVertexArrays(1, &vertexArrayObj);

	//setup and wire up position buffer to packet slot 0

	glCreateBuffers(1, &positionBuffer);
	glNamedBufferStorage(positionBuffer, positionArraySize, positionArray, 0);

	glVertexArrayVertexBuffer(vertexArrayObj, 0, positionBuffer, 0, sizeof(glm::vec4));
	glVertexArrayAttribBinding(vertexArrayObj, 0, 0);
	glVertexArrayAttribFormat(vertexArrayObj, 0, 4, GL_FLOAT, GL_FALSE, 0);
	glEnableVertexArrayAttrib(vertexArrayObj, 0);


	//setup and wire up colour buffer to packet slot 1
	
	glCreateBuffers(1, &colourBuffer);
	glNamedBufferStorage(colourBuffer, colourArraySize, colourArray, 0);

	glVertexArrayVertexBuffer(vertexArrayObj, 1, colourBuffer, 0, sizeof(glm::vec4));
	glVertexArrayAttribBinding(vertexArrayObj, 1, 1);
	glVertexArrayAttribFormat(vertexArrayObj, 1, 4, GL_FLOAT, GL_FALSE, 0);
	glEnableVertexArrayAttrib(vertexArrayObj, 1);


	//setup and wire up height buffer to packet slot 5
	glCreateBuffers(1, &heightBuffer);
	glNamedBufferStorage(heightBuffer, heightArraySize, heightArray, 0);

	glVertexArrayVertexBuffer(vertexArrayObj, 5, heightBuffer, 0, sizeof(float));
	glVertexArrayAttribBinding(vertexArrayObj, 5, 5);
	glVertexArrayAttribFormat(vertexArrayObj, 5, 1, GL_FLOAT, GL_FALSE, 0);
	glEnableVertexArrayAttrib(vertexArrayObj, 5);

	//setup index buffer
	glCreateBuffers(1, &indexBuffer);
	glNamedBufferStorage(indexBuffer, indexArraySize, indexArray, 0);

	glVertexArrayElementBuffer(vertexArrayObj, indexBuffer);

	engine = new RandomEngine(0.0f, 1.0f);

	const GLuint rndSize = 1;
	GLuint numBytes = rndSize * sizeof(float);
	float* randomBuffer = (float*)malloc(numBytes);
	for (GLuint i = 0; i < rndSize; ++i)
		randomBuffer[i] = engine->getRandomValue();

	// Setup SSO
	glGenBuffers(1, &randomArraySSO);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, randomArraySSO);
	glBufferData(GL_SHADER_STORAGE_BUFFER, numBytes, randomBuffer, GL_STATIC_READ);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, randomArraySSO);

	//cleanup
	free(positionArray);
	free(colourArray);
	free(heightArray);
	free(indexArray);

	//setup shader
	grassShader = setupShaders(string("shaders\\grass.vs.txt"), string("shaders\\grass.gs.txt"), string("shaders\\grass.fs.txt"));

}

void GrassField::render(const glm::mat4& viewProjMatrix) {

	glUseProgram(grassShader);
	static GLint mvpLocation = glGetUniformLocation(grassShader, "mvpMatrix");
	glUniformMatrix4fv(mvpLocation, 1, GL_FALSE, (const GLfloat*)&viewProjMatrix);

	static GLint rndLocation = glGetUniformLocation(grassShader, "random");
	GLint r = GLint(engine->getRandomValue() * 1023.0f);
	glUniform1i(rndLocation, r);

	float time = clock.gameTimeDelta();
	static GLint deltaTime = glGetUniformLocation(grassShader, "deltaTime");;
	glUniform1f(deltaTime, time);

	glBindVertexArray(vertexArrayObj);
	glDrawElements(GL_POINTS, numPoints, GL_UNSIGNED_INT, (const GLvoid*)0);

}


