#include "flare/flare.h"

void clearTexture(flare::asset::Texture *texture) {

	if (texture->id != 0) {

		glDeleteTextures(1, &texture->id);
		texture->id = 0;
	}
}

void flare::asset::Texture::_load() {

	clearTexture(this);

	glGenTextures(1, &id);
	glBindTexture(GL_TEXTURE_2D, id);

	glGenerateMipmap(GL_TEXTURE_2D);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	/** @todo This needs all kinds of checking */
	float amount = 4.0f;
	print::d("Anisotropic: %f", amount);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, amount);

	if (name != "") {
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

		byte bytesPerPixel = textureData[offset];
		offset += sizeof(byte);

		/** @todo Check if color space is linear or sRGB */
		if (bytesPerPixel == 0x04) {

			glTexImage2D(GL_TEXTURE_2D, 0, GL_SRGB_ALPHA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, textureData + offset);
		} else if (bytesPerPixel == 0x03) {

			glTexImage2D(GL_TEXTURE_2D, 0, GL_SRGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, textureData + offset);
		} else {

			print::w("'%s' contains an invalid amount of bytes per pixel (%X)", name.c_str(), bytesPerPixel);
		}

		// delete[] textureData;
		allocator::make_delete_array<byte>(*getState()->proxyAllocators.flux, textureData);
	} else {

		print::d("Generating empty texture");

		byte textureData[] =  {0x00, 0x00, 0x00, 0x00};

		glTexImage2D(GL_TEXTURE_2D, 0, GL_SRGB_ALPHA, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, textureData);

	}
}

flare::asset::Texture::~Texture() {

	clearTexture(this);
}
