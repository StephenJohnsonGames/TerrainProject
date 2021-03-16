
#include <pch.h>
#include "TexturedQuadModel.hpp"


using namespace std;


// Geometry data for textured quad (this is rendered directly as a triangle strip)

static float quadPositionArray[] = {

	-0.5f, -0.5f, 0.0f, 1.0f,
	0.5f, -0.5f, 0.0f, 1.0f,
	-0.5f, 0.5f, 0.0f, 1.0f,
	0.5f, 0.5f, 0.0f, 1.0f
};

static float quadTextureCoordArray[] = {

	0.0f, 1.0f,
	1.0f, 1.0f,
	0.0f, 0.0f,
	1.0f, 0.0f
};



//
// Private API
//

void TexturedQuadModel::loadShader() {

	// setup shader for textured quad
	quadShader = setupShaders(string("Shaders\\basic_texture.vs.txt"), string("")
		, string("Shaders\\basic_texture.fs.txt"));
}


void TexturedQuadModel::setupVAO() {

	// setup VAO for textured quad object
	glGenVertexArrays(1, &quadVertexArrayObj);
	glBindVertexArray(quadVertexArrayObj);


	// setup vbo for position attribute
	glGenBuffers(1, &quadVertexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, quadVertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quadPositionArray), quadPositionArray, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, (const GLvoid*)0);


	// setup vbo for texture coord attribute
	glGenBuffers(1, &quadTextureCoordBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, quadTextureCoordBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quadTextureCoordArray), quadTextureCoordArray, GL_STATIC_DRAW);

	glVertexAttribPointer(3, 2, GL_FLOAT, GL_TRUE, 0, (const GLvoid*)0);

	// enable vertex buffers for textured quad rendering (vertex positions and colour buffers)
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(3);

	// unbind textured quad VAO
	glBindVertexArray(0);
}


TexturedQuadModel::TexturedQuadModel(const char* texturePath) {

	loadShader();
	setupVAO();

	// Load texture
	texture = fiLoadTexture(texturePath, TextureProperties(true));
}


TexturedQuadModel::TexturedQuadModel(GLuint initTexture) {

	loadShader();
	setupVAO();

	texture = initTexture;
}


TexturedQuadModel::~TexturedQuadModel() {

	// unbind textured quad VAO
	glBindVertexArray(0);

	// unbind VBOs
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	glDeleteBuffers(1, &quadVertexBuffer);
	glDeleteBuffers(1, &quadTextureCoordBuffer);

	glDeleteVertexArrays(1, &quadVertexArrayObj);

	glDeleteShader(quadShader);
}


void TexturedQuadModel::render(cst::ArcballCamera* camera) {

	//TODO - Bring over shaderfactory

	static GLint mvpLocation = glGetUniformLocation(quadShader, "mvpMatrix");
	glUseProgram(quadShader);
	glUniformMatrix4fv(mvpLocation, 1, GL_FALSE, (const GLfloat*) & (camera));

	//Translate
	glm::mat4 Tr = glm::translate(glm::mat4(1.0f), glm::vec3(1.5f, -0.42f, 1.5f));
	static GLint translate = glGetUniformLocation(quadShader, "translate");
	glUseProgram(quadShader);
	glUniformMatrix4fv(translate, 1, GL_FALSE, (const GLfloat*)& Tr);

	//Rotate
	glm::mat4 Ro = glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	static GLint rotate = glGetUniformLocation(quadShader, "rotate");
	glUseProgram(quadShader);
	glUniformMatrix4fv(rotate, 1, GL_FALSE, (const GLfloat*)& Ro);

	//Scale
	glm::mat4 Sc = glm::scale(glm::mat4(1.0f), glm::vec3(3.0f, 3.0f, 3.0f));
	static GLint scale = glGetUniformLocation(quadShader, "scale");
	glUseProgram(quadShader);
	glUniformMatrix4fv(scale, 1, GL_FALSE, (const GLfloat*)& Sc);

	//view and proj matrices
	glm::mat4 viewTransform = camera->viewTransform();
	glm::mat4 projectionTransform = camera->projectionTransform();

	static GLint viewLocation = glGetUniformLocation(quadShader, "viewMatrix");
	glUseProgram(quadShader);
	glUniformMatrix4fv(viewLocation, 1, GL_FALSE, (const GLfloat*)& viewTransform);

	static GLint projLocation = glGetUniformLocation(quadShader, "projMatrix");
	glUseProgram(quadShader);
	glUniformMatrix4fv(projLocation, 1, GL_FALSE, (const GLfloat*)& projectionTransform);

	//depth
	float nearPlaneDist = camera->getViewFrustum().nearPlaneDistance();
	float farPlaneDist = camera->getViewFrustum().farPlaneDistance();

	static GLint nearLocation = glGetUniformLocation(quadShader, "near");
	glUseProgram(quadShader);
	glUniform1f(nearLocation, nearPlaneDist);

	static GLint farLocation = glGetUniformLocation(quadShader, "far");
	glUseProgram(quadShader);
	glUniform1f(farLocation, farPlaneDist);


	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);

	glBindVertexArray(quadVertexArrayObj);

	// draw quad directly - no indexing needed
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

	// unbind VAO for textured quad
	glBindVertexArray(0);
}


