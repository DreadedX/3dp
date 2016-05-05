#ifndef FLARE_ENGINE_H
#define FLARE_ENGINE_H

namespace flare {

	/** @brief Stores the engine settings, it is initialized with the default engine settings */
	struct Settings {

		/** @brief Window name */
		const char *name = "flare engine";
		/** @brief Window resolution */
		glm::ivec2 resolution = glm::ivec2(1280, 720);
		/** @brief Sensitity of the mouselook
			@todo Camera controls should not be part of the engine */
		float mouseSensitivity = 1.0f;

		/** @brief All possible swap values */
		enum {
			/** @brief No v-sync */
			VSYNC_OFF = 0,
			/** @brief Normal v-sync */
			VSYNC_ON,
			/** @brief Half refresh rate v-sync */
			VSYNC_HALF
		};
		/** @brief V-Sync settings */
		int swap = VSYNC_OFF;
	};

	/** @brief This function is called to initialize the Flare Engine
		*/
	void init();
	/** @brief Check if the engine should still be running
		@return True if the engine should still be running */
	bool isRunning();
	/** @brief Execute all logic and render everything to the screen */
	void update();

	/** @brief Request the engine settings
		@return A pointer to the engine settings */
	Settings *getSettings();

	GLFWwindow *getWindow();

	/** @brief Called after the ending the main loop, makes sure everything get properly unloaded
		@param errorCode - The error code the application should give after shutdown */
	void terminate(int errorCode = 0);
}

#endif

