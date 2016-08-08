#ifndef TEXTURE_H
#define TEXTURE_H

#include <GL/gl.h>

#include "flare/assets.h"

namespace flare {

	namespace asset {

		/** @brief Asset struct containing texture info */
		struct Texture : Asset {

			~Texture();

			/** @brief Texture id */
			GLuint id = 0;

			/** @brief Load texture */
			void _load() override;
		};
	}
}

#endif

