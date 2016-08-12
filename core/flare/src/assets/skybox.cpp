#include <GL/glew.h>

#include "extra/array.h"
#include "flux/file.h"
#include "flux/read.h"
#include "flare/engine.h"
#include "flare/assets/skybox.h"

void clearSkybox(flare::asset::Skybox *skybox) {

	if (skybox->id != 0) {

		glDeleteTextures(1, &skybox->id);
		skybox->id = 0;
	}
}

void flare::asset::Skybox::_load() {

	clearSkybox(this);

	Array<std::string> faces;

	faces.add("core/skybox/polar/right");
	faces.add("core/skybox/polar/left");
	faces.add("core/skybox/polar/top");
	faces.add("core/skybox/polar/bottom");
	faces.add("core/skybox/polar/front");
	faces.add("core/skybox/polar/back");

	glGenTextures(1, &id);

	glBindTexture(GL_TEXTURE_CUBE_MAP, id);
	for (GLuint i = 0; i < faces.size(); ++i) {

		flux::File *textureFile = flux::read::get(faces[i]);
		byte *textureData = textureFile->load();

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

		// Compenstate for bytes per pixel thing
		offset += sizeof(byte);

		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, textureData + offset);

		// delete[] textureData;
		allocator::make_delete_array<byte>(*getState()->proxyAllocators.flux, textureData);
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
}

flare::asset::Skybox::~Skybox() {

	clearSkybox(this);
}

