#include "RandomEngine.hpp"
#include "GUClock.hpp"

class GrassField {


private:

	GLuint			vertexArrayObj;
	GLuint			numPoints;
	GLuint			positionBuffer;
	GLuint			colourBuffer;
	GLuint			heightBuffer;
	GLuint			indexBuffer;

	GLuint			texture;
	GLuint			grassShader;

	RandomEngine* engine;
	GLuint							randomArraySSO; //shader storage object
	cst::GUClock		clock;


public:


	GrassField();
	~GrassField();

	void render(const glm::mat4& viewProjMatrix);

	GrassField(const GLuint numPoints);

};