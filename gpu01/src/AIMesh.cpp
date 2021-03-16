#include <pch.h>
#include "AIMesh.hpp"


AIMesh::AIMesh(const aiScene* scene, const int meshIndex) {

#if 0
	// Material test
	if (scene->HasMaterials()) {

		std::cout << "Materials defined!" << std::endl;
		std::cout << scene->mNumMaterials << " defined!" << std::endl;

		for (int i = 0; i < scene->mNumMaterials; ++i) {

			aiMaterial *material = scene->mMaterials[i];

			aiColor3D rgbColour;

			material->Get(AI_MATKEY_COLOR_DIFFUSE, rgbColour);
			std::cout << "Material " << i << " diffuse = " << rgbColour.r << ", " << rgbColour.g << ", " << rgbColour.b << std::endl;
		}
	}
#endif

	// Setup VAO for mesh
	glGenVertexArrays(1, &meshVAO);
	glBindVertexArray(meshVAO);

	// Setup VBO for vertex position data
	glGenBuffers(1, &meshVertexPosBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, meshVertexPosBuffer);
	glBufferData(GL_ARRAY_BUFFER, scene->mMeshes[meshIndex]->mNumVertices * sizeof(aiVector3D), scene->mMeshes[meshIndex]->mVertices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (const GLvoid*)0);
	glEnableVertexAttribArray(0);

	// Setup VBO for texture coordinate data (use uvw channel 0 only here)
	const GLuint uvwChannel = 0;
	glGenBuffers(1, &meshTexCoordBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, meshTexCoordBuffer);
	glBufferData(GL_ARRAY_BUFFER, scene->mMeshes[meshIndex]->mNumVertices * sizeof(aiVector3D), scene->mMeshes[meshIndex]->mTextureCoords[uvwChannel], GL_STATIC_DRAW);
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_TRUE, 0, (const GLvoid*)0);
	glEnableVertexAttribArray(3);

	// Setup VBO for normal vector data
	glGenBuffers(1, &meshNormalBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, meshNormalBuffer);
	glBufferData(GL_ARRAY_BUFFER, scene->mMeshes[meshIndex]->mNumVertices * sizeof(aiVector3D), scene->mMeshes[meshIndex]->mNormals, GL_STATIC_DRAW);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_TRUE, 0, (const GLvoid*)0);
	glEnableVertexAttribArray(2);

	// Setup VBO for mesh index buffer (face index array)

	// Setup contiguous array
	const GLuint numBytes = scene->mMeshes[meshIndex]->mNumFaces * 3 * sizeof(GLuint);
	GLuint *faceIndexArray = (GLuint*)malloc(numBytes);

	GLuint *dstPtr = faceIndexArray;
	for (unsigned int f = 0; f < scene->mMeshes[meshIndex]->mNumFaces; ++f, dstPtr += 3) {

		memcpy_s(dstPtr, 3 * sizeof(GLuint), scene->mMeshes[meshIndex]->mFaces[f].mIndices, 3 * sizeof(GLuint));
	}

	glGenBuffers(1, &meshFaceIndexBuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, meshFaceIndexBuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, numBytes, faceIndexArray, GL_STATIC_DRAW);

	// Unbind VAO
	glBindVertexArray(0);

	numFaces = scene->mMeshes[meshIndex]->mNumFaces;
}

void AIMesh::render(void) {

	glBindVertexArray(meshVAO);
	glDrawElements(GL_TRIANGLES, numFaces * 3, GL_UNSIGNED_INT, (const GLvoid*)0);
}

void AIMesh::renderInstanced(GLsizei numInstances) {

	glBindVertexArray(meshVAO);
	glDrawElementsInstanced(GL_TRIANGLES, numFaces * 3, GL_UNSIGNED_INT, (const void*)0, numInstances);
}


