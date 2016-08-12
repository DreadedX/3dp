#ifndef SHADER_H
#define SHADER_H

#include <unordered_map>
#include <tuple>

#include <GL/gl.h>

#include "flare/assets.h"
#include "flare/assets/texture.h"

namespace flare::asset {

	/** @brief Asset struct containing shader info
	  @todo Find a better solution to storing all attrib and uniform locators (Maybe an array and using an enum to get the correct value) */
	struct Shader : Asset {

		/** @brief Shader id */
		GLuint id = 0;

		~Shader();

		std::unordered_map<std::string, int> textures;
		std::unordered_map<std::string, int> locations;

		/** @brief Load shader */
		void _load() override;

		void use();

		/** @todo Needs better names */
		void addTexture(const char *textureName, int textureUnit);
		int getTexture(const char *textureName);

		void addLocation(const char *locationName);
		int getLocation(const char *locationName);
	};
}

#endif

