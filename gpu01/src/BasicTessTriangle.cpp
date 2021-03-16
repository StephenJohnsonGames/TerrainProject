#include <pch.h>
#include "BasicTessTriangle.hpp"

using namespace std;

static glm::vec4 positionArray[] = {

	glm::vec4(-1.0f, -1.0f, 0.0f, 1.0f),
	glm::vec4(1.0f, -1.0f, 0.0f, 1.0f),
	glm::vec4(0.0f, 1.0f, 0.0f, 1.0f)

};

static glm::vec4 colourArray[] = {

	glm::vec4(0.0f, 0.0f, 1.0f, 1.0f),
	glm::vec4(0.0f, 1.0f, 0.0f, 1.0f),
	glm::vec4(1.0f, 0.0f, 0.0f, 1.0f)

};

static glm::vec2 textureCoordArray[] = {

	glm::vec2(0.0f, 0.0f),
	glm::vec2(1.0f, 0.0f),
	glm::vec2(0.5f, 1.0f)

};

static unsigned int indexArray[] = {

	0, 1, 2

};

//Constructor

BasicTessTriangle::BasicTessTriangle() {

	//pos
	glCreateVertexArrays(1, &vao);

	glCreateBuffers(1, &positionBuffer);
	glNamedBufferStorage(positionBuffer, sizeof(positionArray), positionArray, 0);

	glVertexArrayVertexBuffer(vao, 0, positionBuffer, 0, sizeof(glm::vec4));
	glVertexArrayAttribBinding(vao, 0, 0);
	glVertexArrayAttribFormat(vao, 0, 4, GL_FLOAT, GL_FALSE, 0);
	glEnableVertexArrayAttrib(vao, 0);

	//colour
	glCreateBuffers(1, &colourBuffer);
	glNamedBufferStorage(colourBuffer, sizeof(colourArray), colourArray, 0);

	glVertexArrayVertexBuffer(vao, 1, colourBuffer, 0, sizeof(glm::vec4));
	glVertexArrayAttribBinding(vao, 1, 1);
	glVertexArrayAttribFormat(vao, 1, 4, GL_FLOAT, GL_FALSE, 0);
	glEnableVertexArrayAttrib(vao, 1);

	//texture
	glCreateBuffers(1, &texCoordBuffer);
	glNamedBufferStorage(texCoordBuffer, sizeof(textureCoordArray), textureCoordArray, 0);
	glVertexArrayVertexBuffer(vao, 2, texCoordBuffer, 0, sizeof(glm::vec2));
	glVertexArrayAttribBinding(vao, 2, 2);
	glVertexArrayAttribFormat(vao, 2, 2, GL_FLOAT, GL_FALSE, 0);
	glEnableVertexArrayAttrib(vao, 2);

	//setup index buffer

	glCreateBuffers(1, &indexBuffer);
	glNamedBufferStorage(indexBuffer, sizeof(indexArray), indexArray, 0);

	glVertexArrayElementBuffer(vao, indexBuffer);

	shaderProg = GLShaderFactory::CreateProgramObject(

		GLShaderFactory::CreateShader(GL_VERTEX_SHADER, "shaders\\Tessellation\\basic_tess.vs.txt"),
		GLShaderFactory::CreateShader(GL_TESS_CONTROL_SHADER, "shaders\\Tessellation\\basic_tess_tri.tcs.txt"),
		GLShaderFactory::CreateShader(GL_TESS_EVALUATION_SHADER, "shaders\\Tessellation\\basic_tess_tri.tes.txt"),
		//GLShaderFactory::CreateShader(GL_GEOMETRY_SHADER, "shaders\\Tessellationz\basic_tess.gs.txt"),
		GLShaderFactory::CreateShader(GL_FRAGMENT_SHADER, "shaders\\Tessellation\\basic_tess.fs.txt")
	);
}

void BasicTessTriangle::render(const glm::mat4& viewProjMatrix) {

	GLuint shader = shaderProg->useProgramObject();

	glUniformMatrix4fv(glGetUniformLocation(shader, "mvpMatrix"),
		1, 
		GL_FALSE, 
		(const GLfloat*)&viewProjMatrix
	);

	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	glPatchParameteri(GL_PATCH_VERTICES, 3);
	glDrawElements(GL_PATCHES, 3, GL_UNSIGNED_INT, (const void*)0);


}