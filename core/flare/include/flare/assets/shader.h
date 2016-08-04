#ifndef SHADER_H
#define SHADER_H

namespace flare {

	namespace asset {

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

			int getLocation(const char *locationName);
			int getTexture(const char *textureName);
		};
	}
}

#endif

