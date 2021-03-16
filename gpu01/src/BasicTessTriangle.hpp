#include <src/GLProgramObject.hpp>
#include <src/GLShaderFactory.hpp>

class BasicTessTriangle {


	GLuint vao;
	GLuint positionBuffer;
	GLuint colourBuffer;
	GLuint texCoordBuffer;
	GLuint indexBuffer;

	GLProgramObject shaderProg;


public:

	BasicTessTriangle();
	void render(const glm::mat4& viewProjMatrix);

};