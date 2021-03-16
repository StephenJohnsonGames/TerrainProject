
#pragma once

class PointExtrudeModel {

private:

	GLuint					paVertexArrayObj;

	GLuint					paVertexBuffer;
	GLuint					paColourBuffer;

	GLuint					paIndexBuffer;

	GLuint					paShader;

public:

	PointExtrudeModel();

	~PointExtrudeModel();

	void render(const glm::mat4& T);
};
#pragma once
