#ifndef SKYBOX_H
#define SKYBOX_H

#include <GL/gl.h>

#include "flare/assets.h"

namespace flare::asset {

	/** @brief Asset struct containing shader info
	  @todo Find a better solution to storing all attrib and uniform locators (Maybe an array and using an enum to get the correct value) */
	struct Skybox : Asset {

		/** @brief Shader id */
		GLuint id = 0;

		~Skybox();

		/** @brief Load shader */
		void _load() override;
	};
}

#endif

