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

	TextureProperties() {}

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
GLuint fiLoadTexture(const char* filename, const TextureProperties& properties);

// loadCubeMapTexture creates and returns a cubemap texture in OpenGL.  This uses the WIC library.  Six images are loaded - the path to each of the six images is determined by <directory><file_prefix><face><extension>.  The 'directory' parameter represents the path to the containing folder of the texture files in the form "C:\...\Textures\" for example.  The 'file_prefix' parameter represents the first part of the texture filename.  The <face> part of the filename is created within loadCubeMapTexture.  This is of the form "_positive_x" and "_negative_x" for example.  The extension parameter is of the form ".bmp" for example.  All image filenames must be of the same type and follow the same structure.  For example, given the texture files "snow_positive_x.bmp" and "snow_negative_x.bmp", 'file_prefix' would be "snow" and 'extension' would be ".bmp"

GLuint loadCubeMapTexture(
	const std::string& directory,
	const std::string& file_prefix,
	const std::string& file_extension,
	GLenum format,
	GLint minFilter,
	GLint maxFilter,
	const GLfloat anisotropicLevel,
	const GLint wrap_s,
	const GLint wrap_t,
	const GLint wrap_r,
	const bool genMipMaps,
	const bool flipImageY);
