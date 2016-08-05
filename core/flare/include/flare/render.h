#ifndef FLARE_RENDER_H
#define FLARE_RENDER_H

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

	}
}

#endif

