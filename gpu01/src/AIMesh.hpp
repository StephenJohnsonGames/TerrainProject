#pragma once

class AIMesh  {

	GLuint				numFaces;

	GLuint				meshVAO;

	GLuint				meshVertexPosBuffer;
	GLuint				meshTexCoordBuffer;
	GLuint				meshNormalBuffer;
	GLuint				meshFaceIndexBuffer;

public:

	AIMesh(const aiScene* scene, const int meshIndex);
	void render(void);
	void renderInstanced(GLsizei numInstances);
};