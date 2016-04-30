#include "flare/flare.h"

void flare::asset::Texture::_load() {

	if (name == "") {
		return;
	}

	if (id != 0) {

		glDeleteTextures(1, &id);
		id = 0;
	}

	glGenTextures(1, &id);
	glBindTexture(GL_TEXTURE_2D, id);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	// glGenerateMipmap(GL_TEXTURE_2D);
	flux::FileLoad *textureFile = flux::get(name);
	byte *textureData = textureFile->get();

	int width = 0;
	int height = 0;

	uint offset = 0;
	for (uint i = 0; i < sizeof(int); ++i) {
		width += textureData[i] << (i*8);
	}
	offset += sizeof(int);

	for (uint i = 0; i < sizeof(int); ++i) {
		height += textureData[i + offset] << (i*8);
	}
	offset += sizeof(int);

	// byte bytesPerPixel = textureData[offset];
	offset += sizeof(byte);

	/** @todo Check bytes per pixel */
	// Check if color space is linear or sRGB
	glTexImage2D(GL_TEXTURE_2D, 0, GL_SRGB_ALPHA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, textureData + offset);
	// glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
	delete[] textureData;
}
