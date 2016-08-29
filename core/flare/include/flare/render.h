#ifndef FLARE_RENDER_H
#define FLARE_RENDER_H

#include <string>

#include "extra/typedef.h"

namespace flare {

	class GameState;

	/** @brief Render code */
	namespace render {

		/** @brief Initialize OpenGL and renderer */
		void init();

		/** @brief Draw a new frame */
		void draw(GameState *gameState);

		/** @brief Get the frame delta time
			@returns The delta frame time */
		float getDeltaTime();

		void setShaderOutput(std::string texureName, uint textureId);

		uint getShaderOutput(std::string texureName);

		void quad();

		void setTexture(uint textureLocation, uint textureUniform);
	}
}

#endif

