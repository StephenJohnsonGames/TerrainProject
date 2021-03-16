#pragma once

class AIMesh;

// Model a collection of meshes into a coherent scene.
class AIScene {

	GLuint			numMeshes = 0;
	AIMesh**		meshArray = nullptr;

public:

	AIScene(const std::string& filename, unsigned int flags);
	void render(void);
};