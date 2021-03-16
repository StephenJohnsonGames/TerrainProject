#include <pch.h>
#include "ProceduralTexture.hpp"

using namespace std;


ProceduralTexture::ProceduralTexture() {

	// Setup aligned data structures to store the uniform buffer data to pass to the shader
	struct alignas(16) aligned16_uint {
		GLuint value;
	};

	struct alignas(16) aligned16_vec2 {
		glm::vec2 value;
	};

	struct alignas(16) PerlinModel {

		alignas(4) GLuint N = 0x100;
		alignas(4) GLuint Nmask = 0xFF;
		alignas(16) aligned16_uint iTable[256];
		alignas(16) aligned16_vec2 vTable[256];
	
	};


	// Setup table data

	PerlinModel* perlinNoiseModel = new PerlinModel();

	perlinNoiseModel->N = 256;
	perlinNoiseModel->Nmask = 255;

	random_device rd;
	mt19937 mt = mt19937(rd());
	uniform_int_distribution<GLuint> indexDist(0, 255);

	float theta = 0.0f;
	float pi2 = glm::pi<float>() * 2.0f;
	float angleStep = pi2 / (float)perlinNoiseModel->N;

	for (GLuint i = 0; i < 256; i++) {

		perlinNoiseModel->iTable[i].value = indexDist(mt);

		// unit length gradient vector
		perlinNoiseModel->vTable[i].value.x = cos(theta);
		perlinNoiseModel->vTable[i].value.y = sin(theta); 

		theta += angleStep;
	}


	// Setup buffer object for perlin model
	glCreateBuffers(1, &perlinModelBuffer);
	cout << "sizeof PerlinModel = " << sizeof(PerlinModel) << endl;
	glNamedBufferStorage(perlinModelBuffer, sizeof(PerlinModel), perlinNoiseModel, 0);


	// Load compute shader
	computeShader = GLShaderFactory::CreateProgramObject(

		GLShaderFactory::CreateShader(GL_COMPUTE_SHADER, string("shaders\\Compute\\perlin_noise.cs.txt"))
	);
}


// The constructor doesn't create the texture - do we that with a call to generateTexture.  Create a new texture object and return the object id
GLuint ProceduralTexture::generateTexture(GLuint w, GLuint h) {

	// Setup texture
	GLuint newTexture;

	glGenTextures(1, &newTexture);
	glBindTexture(GL_TEXTURE_2D, newTexture);

	// Setup texture data - but pass null for last parameter.  OpenGL creates the texture image array in memory but no data is passed into this.  We'll provide the content when we run our shader.
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
	
	// Setup texture properties
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, 4);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	// Bind shader
	GLuint shader = computeShader->useProgramObject();

	// Bind the perlin uniform buffer
	glBindBufferBase(GL_UNIFORM_BUFFER, 0, perlinModelBuffer);
	// Bind the texture to image object binding point 0
	glBindImageTexture(0, newTexture, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);

	// Calculate number of groups - since the shader declares 16x16 workgroup sizes, we divide the total image / texture size by 16 to get the number of workgroups on the x and y dimensions.  Since it's only a 2D dataset, z has a workgroup size of 1.
	GLuint numGroupsX = w >> 4;
	GLuint numGroupsY = h >> 4;
	GLuint numGroupsZ = 1;

	// Run our compute shader to fill in the texture data
	glDispatchCompute(numGroupsX, numGroupsY, numGroupsZ);
	
	// Return the new texture object to the caller
	return newTexture;
}