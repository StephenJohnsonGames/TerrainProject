
#include <pch.h>
#include "texture_loader.hpp"
#include <FreeImagePlus.h>


using namespace std;


#pragma region MipMap processing

static CGMipmapGenMode mipmapGenMode = CG_NO_MIPMAP_GEN;
static bool mipmapModeInitialised = false;

static void initialiseMipmapMode() {

	if (glewIsSupported("GL_ARB_framebuffer_object"))
		mipmapGenMode = CG_CORE_MIPMAP_GEN;
	else if (glewIsSupported("GL_EXT_framebuffer_object"))
		mipmapGenMode = CG_EXT_MIPMAP_GEN;
	else
		mipmapGenMode = CG_NO_MIPMAP_GEN;

	mipmapModeInitialised = true;
}

#pragma endregion


#pragma region FreeImagePlus texture loader

GLuint fiLoadTexture(const char *filename, const TextureProperties& properties) {

	BOOL				fiOkay = FALSE;
	GLuint				newTexture = 0;
	fipImage			I;

	fiOkay = I.load(filename);

	if (!fiOkay) {

		cout << "FreeImagePlus: Cannot open image file.\n";
		return 0;
	}

	if (properties.flipImageY) {

		fiOkay = I.flipVertical();
	}

	fiOkay = I.convertTo32Bits();

	if (!fiOkay) {

		cout << "FreeImagePlus: Conversion to 24 bits successful.\n";
		return 0;
	}

	auto w = I.getWidth();
	auto h = I.getHeight();

	BYTE *buffer = I.accessPixels();

	if (!buffer) {

		cout << "FreeImagePlus: Cannot access bitmap data.\n";
		return 0;
	}

	glGenTextures(1, &newTexture);
	glBindTexture(GL_TEXTURE_2D, newTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, properties.internalFormat, w, h, 0, GL_BGRA, GL_UNSIGNED_BYTE, buffer);

	// Setup default texture properties
	if (newTexture) {

		// Verify we don't use GL_LINEAR_MIPMAP_LINEAR which has no meaning in non-mipmapped textures.  If not set, default to GL_LINEAR (bi-linear) filtering.
		GLint minFilter = (!properties.genMipMaps && properties.minFilter == GL_LINEAR_MIPMAP_LINEAR) ? GL_LINEAR : properties.minFilter;
		GLint maxFilter = (!properties.genMipMaps && properties.maxFilter == GL_LINEAR_MIPMAP_LINEAR) ? GL_LINEAR : properties.maxFilter;

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilter);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, maxFilter);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, properties.anisotropicLevel);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, properties.wrap_s);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, properties.wrap_t);

		// Initialise mipmap creation method based on supported extensions
		if (!mipmapModeInitialised)
			initialiseMipmapMode();

		if (properties.genMipMaps && mipmapGenMode != CG_NO_MIPMAP_GEN) {

			if (mipmapGenMode == CG_CORE_MIPMAP_GEN)
				glGenerateMipmap(GL_TEXTURE_2D);
			else if (mipmapGenMode == CG_EXT_MIPMAP_GEN)
				glGenerateMipmapEXT(GL_TEXTURE_2D);
		}
	}

	return newTexture;
}

#pragma endregion

