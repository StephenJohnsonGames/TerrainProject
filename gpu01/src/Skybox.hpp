#pragma once

#include "ArcballCamera.hpp"

class SkyBox {

private:

	GLuint					vao;

	GLuint					positionBuffer;
	GLuint					normalBuffer;
	GLuint					indexBuffer;

	GLuint					shader;

	GLuint					texture;

public:

	SkyBox(const std::string& cubemapPath, const std::string& cubemapName, const std::string& cubemapExtension);

	~SkyBox();

	GLuint skyboxTexture();
	void setSkyboxTexture(const GLuint newTexture);

	void render(cst::ArcballCamera* camera);
};
