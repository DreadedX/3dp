#ifndef FLARE_INPUT_H
#define FLARE_INPUT_H

namespace flare {
    namespace input {

	bool keyCheck(int key);
	void keySet(int key, bool state);
	void _keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods);

	struct Mouse {

	    glm::vec2 position = glm::vec2(0.0f, 0.0f);
	    float yaw = -90.0f;
	    float pitch = 0.0f;

	    bool _first = true;
	};

	void _mouseCallback(GLFWwindow *window, double xpos, double ypos);
	Mouse *getMouse();
    }
}

#endif

