#include <pch.h>
#include "AIScene.hpp"
#include "AIMesh.hpp"

using namespace std;

AIScene::AIScene(const string& filename, unsigned int flags) {

	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(filename, flags);

	numMeshes = scene->mNumMeshes;
	meshArray = (AIMesh**)malloc(numMeshes * sizeof(AIMesh*));

	for (unsigned int i = 0; i < numMeshes; ++i) {

		meshArray[i] = new AIMesh(scene, i);
	}
}


void AIScene::render(void) {

	for (unsigned int i = 0; i < numMeshes; ++i) {

		meshArray[i]->render();
	}
}

void AIScene::renderInstanced(GLsizei numInstances) {

	for (unsigned int i = 0; i < numMeshes; ++i) {

		meshArray[i]->renderInstanced(numInstances);
	}
}
