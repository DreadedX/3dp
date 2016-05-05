#ifndef FLARE_INPUT_H
#define FLARE_INPUT_H

namespace flare {

	/** @brief Input handling */
	namespace input {

		/** @brief For now makes sure the mouse delta is 0 */
		void update();

		/** @brief Check the state of a key
			@param key - GLFW key id for the key you want to check
			@returns True if the key is pressed */
		bool keyCheck(int key);

		/** @brief Set the state of a key
			This is used to make single press functions
			@param key - GLFW key id for the key you want to set
			@param state - True = key is pressed, False = key is not pressed */
		void keySet(int key, bool state);

		/** @brief Install key callback
			@note For internal usage only */
		void _keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods);

		/** @brief Stores mouse info
			@todo This is propably not really needed, especially when pich and yaw are moved */
		struct Mouse {

			/** @brief Current mouse location, relative to the window */
			glm::vec2 position = glm::vec2(0.0f, 0.0f);
		};

		/** @brief Install mouse callback
			@note For internal usage only */
		void _mouseCallback(GLFWwindow *window, double xpos, double ypos);

		/** @brief Get a pointer to the mouse object
			@returns Pointer to the mouse object */
		Mouse *getMouse();
	}
}

#endif

