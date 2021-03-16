#pragma once

#include "GLShaderFactory.hpp"

//
// Model a Perlin noise-based procedural texture
//

class ProceduralTexture {

	// Buffer to store uniform data that configures the noise
	GLuint			perlinModelBuffer;

	// Shader program
	GLProgramObject computeShader;

public:

	// Constructor to setup the generator
	ProceduralTexture();

	// The constructor doesn't create the texture - do we that with a call to generateTexture.  Create a new texture object and return the object id
	GLuint generateTexture(GLuint w, GLuint h);

};