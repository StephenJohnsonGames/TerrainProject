#pragma once


// Model a simple textured quad oriented to face along the +z axis (so the textured quad faces the viewer in (right-handed) eye coordinate space.  The quad is modelled using VBOs and VAOs and rendered using the basic texture shader in Resources\Shaders\basic_texture.vs and Resources\Shaders\basic_texture.fs

class TexturedQuadModel {

private:

	GLuint					quadVertexArrayObj;

	GLuint					quadVertexBuffer;
	GLuint					quadTextureCoordBuffer;

	GLuint					quadShader;

	GLuint					texture;

	//
	// Private API
	//

	void loadShader();
	void setupVAO();


	//
	// Public API
	//

public:

	TexturedQuadModel(const char *texturePath, const TextureProperties& properties);
	TexturedQuadModel(GLuint initTexture);

	~TexturedQuadModel();

	GLuint getTexture();

	void render(const glm::mat4& T);
};
