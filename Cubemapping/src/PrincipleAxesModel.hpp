
#pragma once

class PrincipleAxesModel {

private:

	GLuint					paVertexArrayObj;

	GLuint					paVertexBuffer;
	GLuint					paColourBuffer;

	GLuint					paIndexBuffer;

	GLuint					paShader;

public:

	PrincipleAxesModel();

	~PrincipleAxesModel();

	void render(const glm::mat4& T);
};
