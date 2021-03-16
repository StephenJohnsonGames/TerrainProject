#include "ArcballCamera.hpp"
#include "GLShaderFactory.hpp"
#include "GLProgramObject.hpp"

class cst::ArcballCamera;

class BasicTerrain {


private:


	GLuint			vertexArrayObj;

	GLuint			extent;
	GLuint			numPoints;
	GLuint			numIndices;

	GLuint			positionBuffer;
	GLuint			indexBuffer;
	GLuint			texBuffer;
	
	//normals require **inverse transpose** matrix instead of TSR matrix when applying transformations
	GLuint			normalBuffer;
	GLuint			tangentBuffer;

	//also called the biNormal
	GLuint			biTangentBuffer;

	GLuint			wireframeShader;
	GLuint			surfaceShader;

	GLuint			grassTexture;
	GLuint			rockTexture;
	GLuint			snowTexture;
	GLuint			darkRockTexture;

	GLuint			texModelBuffer;

	//GLuint64		grassHandle;
	//GLuint64		rockHandle;
	//GLuint64		snowHandle;

	//GLProgramObject shaderProgram;

public:


	BasicTerrain();
	~BasicTerrain();

	BasicTerrain(const GLuint extent);

	void render(cst::ArcballCamera* camera);


};