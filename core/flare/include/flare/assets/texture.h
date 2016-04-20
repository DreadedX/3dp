#ifndef TEXTURE_H
#define TEXTURE_H

namespace flare {

	namespace asset {

		/** @brief Asset struct containing texture info */
		struct Texture : Asset {

			/** @brief Texture id */
			GLuint id = 0;

			/** @brief Load texture */
			void _load() override;
		};
	}
}

#endif

