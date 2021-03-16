#pragma once

// Structure to define properties for new textures
struct TextureProperties {

	GLint		internalFormat = GL_RGBA8;
	GLint		minFilter = GL_LINEAR;
	GLint		maxFilter = GL_LINEAR;
	GLfloat		anisotropicLevel = 0.0f;
	GLint		wrap_s = GL_REPEAT;
	GLint		wrap_t = GL_REPEAT;
	bool		genMipMaps = FALSE;
	bool		flipImageY = FALSE;

	TextureProperties(bool flipImageY) {

		this->flipImageY = flipImageY;
	}

	TextureProperties(GLint format) {

		internalFormat = format;
	}
	
	TextureProperties(GLint format, GLint minFilter, GLint maxFilter, GLfloat anisotropicLevel, GLint wrap_s, GLint wrap_t, bool genMipMaps, bool flipImageY) {
	
		this->internalFormat = format;
		this->minFilter = minFilter;
		this->maxFilter = maxFilter;
		this->anisotropicLevel = anisotropicLevel;
		this->wrap_s = wrap_s;
		this->wrap_t = wrap_t;
		this->genMipMaps = genMipMaps;
		this->flipImageY = flipImageY;
	}
};

enum CGMipmapGenMode { CG_NO_MIPMAP_GEN = 0, CG_CORE_MIPMAP_GEN, CG_EXT_MIPMAP_GEN };

// FreeImage texture loader
GLuint fiLoadTexture(const char *filename, const TextureProperties& properties);
