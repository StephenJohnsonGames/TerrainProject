#include "../ArcballCamera.hpp"
#include "../GLProgramObject.hpp"
#include "../GLShaderFactory.hpp"

class GrassPatches;

// Forward declaration for cst::ArcballCamera
namespace cst {

	class ArcballCamera;
};

class PatchTerrain {


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

	GLProgramObject shaderProgram;

	// On-terrain features
	GrassPatches*	grassPatches;

public:


	PatchTerrain();
	~PatchTerrain();

	PatchTerrain(const GLuint extent, const glm::vec2& domainCoord, const glm::vec2& domainExtent, int numOctaves);

	void render(cst::ArcballCamera* camera);


};