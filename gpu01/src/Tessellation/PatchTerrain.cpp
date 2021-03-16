#include <pch.h>
#include "PatchTerrain.hpp"
#include <src/PerlinNoise.hpp>
#include <src/GrassPatches.hpp>

using namespace std;
using namespace cst;

// ** Model texture data needed for terrain
/*
	From OpenGL Superbible 7th edition: Any type consuming N bytes in a buffer begins on an N-byte boundary within that buffer. That means that standard GLSL types such as int, float, and bool (which are all defined to be 32-bit or 4-byte quantities) begin on multiples of 4 bytes. A vector of these types of length 2 always begins on a 2N-byte boundary. For example, a vec2, which is 8 bytes long in memory, always starts on an 8-byte boundary. Three- and four-element vectors always start on a 4N-byte boundary; vec3 and vec4 types start on 16-byte boundaries, for instance. Each member of an array of scalar or vector types (arrays of int or vec3, for example) always starts on a boundary defined by these same rules, but rounded up to the alignment of a vec4. In particular, this means that arrays of anything but vec4 (and N × 4 matrices) won’t be tightly packed, but instead will have a gap between each of the elements. Matrices are essentially treated like short arrays of vectors, and arrays of matrices are treated like very long arrays of vectors. Finally, structures and arrays of structures have additional packing requirements; the whole structure starts on the boundary required by its largest member, rounded up to the size of a vec4.
	*/

	// C++ will pack array elements in cases such as: alignas(16) GLint myarray[] = { ... };
	// So need to align each element - to do this setup wrapper structure with appropriate alignment.
	// Not ideal as access becomes: auto x = myArray[i].value;
	// However, helps simplify alignment structure of memory footprint
struct alignas(16) int_element16 {
	GLint value;
};

//old way to handle textures
//struct alignas(16) TerrainTexture {
//
//	alignas(8) GLuint64 grassHandle;
//	alignas(8) GLuint64 rockHandle;
//	alignas(8) GLuint64 snowHandle;
//	alignas(8) GLuint64 darkRockHandle;
//
//	alignas(8) glm::vec2 grassScale;
//	alignas(8) glm::vec2 rockScale;
//	alignas(8) glm::vec2 snowScale;
//	alignas(8) glm::vec2 darkRockScale;
//
//};

//newer way to handle textures

struct alignas(16) Texture {

	alignas(8) GLuint64 texture;
	alignas(8) glm::vec2 scale;
};

struct alignas(16) TerrainTexture {

	Texture grass, rock, snow;
};


PatchTerrain::PatchTerrain(const GLuint extent, const glm::vec2& domainCoord, const glm::vec2& domainExtent, int numOctaves) {

	//homework - set up as triangles instead of points , means you have to set up your index array

	PerlinNoise* noiseGen = new PerlinNoise();

	this->extent = extent;

	numPoints = extent * extent;

	GLuint positionArraySize = numPoints * sizeof(glm::vec4);

	//extent - 1 = indices per array size, 2 = 2 triangles per quad, 3 sides per triangle
	GLuint indexArraySize = (extent - 1) * (extent - 1) * 2 * 3 * sizeof(GLuint);
	numIndices = (extent - 1) * (extent - 1) * 2 * 3;
	glm::vec4* positionArray = (glm::vec4*)malloc(positionArraySize);

	glm::vec3* normalArray = (glm::vec3*)malloc(numPoints * sizeof(glm::vec3));
	glm::vec3* tangentArray = (glm::vec3*)malloc(numPoints * sizeof(glm::vec3));
	glm::vec3* biTangentArray = (glm::vec3*)malloc(numPoints * sizeof(glm::vec3));

	//textures
	glm::vec2* texArray = (glm::vec2*)malloc(numPoints * sizeof(glm::vec2));

	//always check malloc allocations are valid
	GLuint* indexArray = (GLuint*)malloc(indexArraySize);


	random_device rd;
	mt19937 mt = mt19937(rd());
	uniform_real_distribution<float> positionDist = uniform_real_distribution<float>(-2.0f, 2.0f);
	uniform_real_distribution<float> heightDist = uniform_real_distribution<float>(-1.0f, 1.0f);

	// Calculate domain step for each vertex...
	glm::vec2 domainStep = glm::vec2(domainExtent.x / float(extent - 1), domainExtent.y / float(extent - 1));

	glm::vec2 dnTotal = glm::vec2(0.0f, 0.0f);

	// Setup lambda (closure) to calculate fractal noise.
	auto fractalLambda = [&](float u, float v)->float {

		float noiseValue = 0.0f;
		glm::vec2 dn = glm::vec2(0.0f, 0.0f);
		float freq = 1.0f;

		for (int f = 0; f < numOctaves; f++, u *= 2.0f, v *= 2.0f, freq *= 2.0f) {

			// Noise with fractal power spectrum (frequency distribution)
			noiseValue += noiseGen->noise(u, v, 1.0f, &dn) * (1.0f / freq);
			dnTotal += dn * (1.0f / freq);
		}

		return noiseValue;
	};

	//populate arrays

	//old way, checking with new fractal noise for instancing

	//float domainStart = 0.0f;
	//float domainEnd = 5.0f;
	//float domainSize = domainEnd - domainStart;
	//float domainStep = domainSize / float(extent);

	//int numOctaves = 6;


	//float v = domainStart;

	//for (GLuint i = 0, z = 0; z < extent; z++, v += domainStep) {
	//	float u = domainStart;
	//	for (GLuint x = 0; x < extent; x++, i++, u += domainStep) {

	//		//old method
	//		/*positionArray[i].x = (float)x * 0.05f;
	//		positionArray[i].y = noiseGen->noise((float)x, (float)z, 0.1f);
	//		positionArray[i].z = (float)z * 0.05f;
	//		positionArray[i].w = 1.0f;*/

	//		//fractal method
	//		glm::vec2 dnTotal = glm::vec2(0.0f, 0.0f);
	//		glm::vec2 dn = glm::vec2(0.0f, 0.0f);

	//		float noiseValue = 0.0f;
	//		float uLocal = u;
	//		float vLocal = v;
	//		float freq = 1.0f;

	//		for (int f = 0; f < numOctaves; f++, uLocal *= 2.0f, vLocal *= 2.0f, freq *= 2.0f) {

	//			//dividing these instead of multiplying them together does some mad
	//			noiseValue += noiseGen->noise(uLocal, vLocal, 1.0f, &dn) * (1.0f / freq);

	//			dnTotal += dn * (1.0f / freq);
	//		}

	//		positionArray[i].x = u;
	//		positionArray[i].y = noiseValue;
	//		positionArray[i].z = v;
	//		positionArray[i].w = 1.0f;

	//		tangentArray[i] = glm::normalize(glm::vec3(1.0f, dnTotal.x, 0.0f));
	//		biTangentArray[i] = glm::normalize(glm::vec3(0.0f, dnTotal.y, 1.0f));
	//		normalArray[i] = glm::normalize(glm::cross(biTangentArray[i], tangentArray[i]));

	//		//		normalized				u component						v compontent
	//		texArray[i] = glm::vec2(float(x) / float(extent - 1), float(z) / float(extent - 1));

	//	}
	//}

	float v = domainCoord.y;
	for (GLuint i = 0, z = 0; z < extent; z++, v += domainStep.y) {

		float u = domainCoord.x;
		for (GLuint x = 0; x < extent; x++, i++, u += domainStep.x) {

			dnTotal = glm::vec2(0.0f, 0.0f);
			float noiseValue = fractalLambda(u, v);

			positionArray[i].x = u;
			positionArray[i].y = noiseValue;
			positionArray[i].z = v;
			positionArray[i].w = 1.0f;

			tangentArray[i] = glm::normalize(glm::vec3(1.0f, dnTotal.x, 0.0f));
			biTangentArray[i] = glm::normalize(glm::vec3(0.0f, dnTotal.y, 1.0f));
			normalArray[i] = glm::normalize(glm::cross(biTangentArray[i], tangentArray[i]));
			texArray[i] = glm::vec2(float(x) / float(extent - 1), float(z) / float(extent - 1));
		}
	}

	GLuint* iPtr = indexArray;
	for (GLuint z = 0; z < (extent - 1); z++) {

		//iPtr shifts at end of for loop for next row
		for (GLuint x = 0; x < (extent - 1); x++, iPtr += 6) {
			//first 3 points in our triangle, from bottom left, up one and right one
			//going clockwise
			iPtr[0] = z * extent + x;
			iPtr[1] = (z + 1) * extent + x;
			iPtr[2] = (z + 1) * extent + x + 1;

			//next triangle
			iPtr[3] = iPtr[0];
			iPtr[4] = iPtr[2];
			iPtr[5] = z * extent + x + 1;
		}
	}

	//GPU structures

	//pos
	glCreateVertexArrays(1, &vertexArrayObj);


	glCreateBuffers(1, &positionBuffer);
	glNamedBufferStorage(positionBuffer, positionArraySize, positionArray, 0);

	glVertexArrayVertexBuffer(vertexArrayObj, 0, positionBuffer, 0, sizeof(glm::vec4));
	glVertexArrayAttribBinding(vertexArrayObj, 0, 0);
	glVertexArrayAttribFormat(vertexArrayObj, 0, 4, GL_FLOAT, GL_FALSE, 0);
	glEnableVertexArrayAttrib(vertexArrayObj, 0);

	//normals
	glCreateBuffers(1, &normalBuffer);
	glNamedBufferStorage(normalBuffer, extent * extent * sizeof(glm::vec3), normalArray, 0);
	glVertexArrayVertexBuffer(vertexArrayObj, 1, normalBuffer, 0, sizeof(glm::vec3));
	glVertexArrayAttribBinding(vertexArrayObj, 1, 1);
	glVertexArrayAttribFormat(vertexArrayObj, 1, 3, GL_FLOAT, GL_FALSE, 0);
	glEnableVertexArrayAttrib(vertexArrayObj, 1);

	//texture
	glCreateBuffers(1, &texBuffer);
	glNamedBufferStorage(texBuffer, extent * extent * sizeof(glm::vec2), texArray, 0);
	glVertexArrayVertexBuffer(vertexArrayObj, 2, texBuffer, 0, sizeof(glm::vec2));
	glVertexArrayAttribBinding(vertexArrayObj, 2, 2);
	glVertexArrayAttribFormat(vertexArrayObj, 2, 2, GL_FLOAT, GL_FALSE, 0);
	glEnableVertexArrayAttrib(vertexArrayObj, 2);

	//setup index buffer

	glCreateBuffers(1, &indexBuffer);
	glNamedBufferStorage(indexBuffer, indexArraySize, indexArray, 0);

	glVertexArrayElementBuffer(vertexArrayObj, indexBuffer);

	//cleanup

	free(positionArray);
	free(indexArray);
	//free(normalArray);

	//setup shader
	//surfaceShader = setupShaders(string("shaders\\terrain.vs.txt"), string(""), string("shaders\\terrain.fs.txt"));
	//wireframeShader = setupShaders(string("shaders\\terrain_wireframe.vs.txt"), string(""), string("shaders\\terrain_wireframe.fs.txt"));

	shaderProgram = GLShaderFactory::CreateProgramObject(
		GLShaderFactory::CreateShader(GL_VERTEX_SHADER, "shaders\\Tessellation\\terrain.vs.txt"),
		GLShaderFactory::CreateShader(GL_TESS_CONTROL_SHADER, "shaders\\Tessellation\\terrain.tcs.txt"),
		GLShaderFactory::CreateShader(GL_TESS_EVALUATION_SHADER, "shaders\\Tessellation\\terrain.tes.txt"),
		GLShaderFactory::CreateShader(GL_FRAGMENT_SHADER, "shaders\\Tessellation\\terrain.fs.txt")
	);

	//load textures

	//texture filtering - anisotropic filtering 8x
	grassTexture = fiLoadTexture("..\\shared_resources\\Textures\\Terrain\\grass.jpg", TextureProperties(GL_SRGB8_ALPHA8, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR, 8.0f, GL_REPEAT, GL_REPEAT, true, true));
	rockTexture = fiLoadTexture("..\\shared_resources\\Textures\\Terrain\\rock.jpg", TextureProperties(GL_SRGB8_ALPHA8, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR, 8.0f, GL_REPEAT, GL_REPEAT, true, true));
	snowTexture = fiLoadTexture("..\\shared_resources\\Textures\\Terrain\\snow.jpg", TextureProperties(GL_SRGB8_ALPHA8, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR, 8.0f, GL_REPEAT, GL_REPEAT, true, true));
	darkRockTexture = fiLoadTexture("..\\shared_resources\\Textures\\Terrain\\dark_rock.jpg", TextureProperties(true));

	

	//old method

	//TerrainTexture textureModel;

	//bind handles
	//textureModel.grassHandle = glGetTextureHandleARB(grassTexture);
	//textureModel.rockHandle = glGetTextureHandleARB(rockTexture);
	//textureModel.snowHandle = glGetTextureHandleARB(snowTexture);
	//textureModel.darkRockHandle = glGetTextureHandleARB(darkRockTexture);

	//textureModel.grassScale = glm::vec2(6.0f, 6.0f);
	//textureModel.rockScale = glm::vec2(6.0f, 6.0f);
	//textureModel.snowScale = glm::vec2(6.0f, 6.0f);
	//textureModel.darkRockScale = glm::vec2(6.0f, 6.0f);

	TerrainTexture T;

	T.grass.texture = glGetTextureHandleARB(grassTexture);
	T.rock.texture = glGetTextureHandleARB(rockTexture);
	T.snow.texture = glGetTextureHandleARB(snowTexture);
	T.grass.scale = glm::vec2(6.0f, 6.0f);
	T.rock.scale = glm::vec2(6.0f, 6.0f);
	T.snow.scale = glm::vec2(6.0f, 6.0f);

	glCreateBuffers(1, &texModelBuffer);
	glNamedBufferStorage(texModelBuffer, sizeof(TerrainTexture), &T, 0);

	//ready for drawing
	glMakeTextureHandleResidentARB(T.grass.texture);
	glMakeTextureHandleResidentARB(T.rock.texture);
	glMakeTextureHandleResidentARB(T.snow.texture);

	grassPatches = new GrassPatches(5000, domainCoord, domainExtent, fractalLambda);
}

void PatchTerrain::render(ArcballCamera* camera) {

	//link handle and sampler in shaders
	glBindBufferBase(GL_UNIFORM_BUFFER, 0, texModelBuffer);

	//glUniform1i64ARB(glGetUniformLocation(surfaceShader, "grassTexture"), grassHandle);
	//glUniform1i64ARB(glGetUniformLocation(surfaceShader, "snowTexture"), snowHandle);
	//glUniform1i64ARB(glGetUniformLocation(surfaceShader, "rockTexture"), rockHandle);

	glm::mat4 projTransform = camera->projectionTransform();
	glm::mat4 viewTransform = camera->viewTransform();
	float nearPlaneDist = camera->getViewFrustum().nearPlaneDistance();
	float farPlaneDist = camera->getViewFrustum().farPlaneDistance();

	glBindVertexArray(vertexArrayObj);

	glPointSize(2.0f);
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	//draw triangles in first
	//glUseProgram(surfaceShader);
	GLuint surfaceShader = shaderProgram->useProgramObject();

	static GLint viewLocation = glGetUniformLocation(surfaceShader, "viewMatrix");
	static GLint projLocation = glGetUniformLocation(surfaceShader, "projMatrix");
	static GLint nearLocation = glGetUniformLocation(surfaceShader, "near");
	static GLint farLocation = glGetUniformLocation(surfaceShader, "far");
	glUniformMatrix4fv(viewLocation, 1, GL_FALSE, (const GLfloat*)&viewTransform);
	glUniformMatrix4fv(projLocation, 1, GL_FALSE, (const GLfloat*)&projTransform);
	glUniform1f(nearLocation, nearPlaneDist);
	glUniform1f(farLocation, farPlaneDist);

	glPatchParameteri(GL_PATCH_VERTICES, 3);
	glDrawElements(GL_PATCHES, numIndices, GL_UNSIGNED_INT, (const GLvoid*)0);

	//then draw line strip
	//glUseProgram(wireframeShader);
	//viewLocation = glGetUniformLocation(wireframeShader, "viewMatrix");
	//projLocation = glGetUniformLocation(wireframeShader, "projMatrix");
	//nearLocation = glGetUniformLocation(wireframeShader, "near");
	//farLocation = glGetUniformLocation(wireframeShader, "far");
	//glUniformMatrix4fv(viewLocation, 1, GL_FALSE, (const GLfloat*)&viewTransform);
	//glUniformMatrix4fv(projLocation, 1, GL_FALSE, (const GLfloat*)&projTransform);
	//glUniform1f(nearLocation, nearPlaneDist);
	//glUniform1f(farLocation, farPlaneDist);
	//glDrawElements(GL_TRIANGLES, numIndices, GL_UNSIGNED_INT, (const GLvoid*)0);

	//glDrawArrays(GL_POINTS, 0, numPoints);

	//render on-surface features
	if (grassPatches) {

		grassPatches->render(camera);
	}
}

