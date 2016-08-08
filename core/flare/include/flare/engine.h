#ifndef FLARE_ENGINE_H
#define FLARE_ENGINE_H

#include "flare/state.h"

namespace flare {

	/** @brief This function is called to initialize the Flare Engine */
	void init();
	/** @brief Check if the engine should still be running
		@return True if the engine should still be running */
	bool isRunning();
	/** @brief Execute all logic and render everything to the screen */
	void update();

	State *getState();

	/** @brief Called after the ending the main loop, makes sure everything get properly unloaded
		@param errorCode - The error code the application should give after shutdown */
	void terminate(int errorCode = 0);
}

#endif

