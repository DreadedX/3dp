#ifndef FLARE_INPUT_H
#define FLARE_INPUT_H

namespace flare {
    namespace input {

	void _keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods);
	bool keyCheck(int key);
	void keySet(int key, bool state);
    }
}

#endif

