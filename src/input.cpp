#include "flare/flare.h"

bool keys[25536] = {false};

void flare::keyCallback(GLFWwindow*, int key, int, int action, int) {

    if (action == GLFW_PRESS) {

	keys[key] = true;
    }

    if (action == GLFW_RELEASE) {

	keys[key] = false;
    }
}

bool flare::keyCheck(int key) {

    return keys[key];
}

void flare::keySet(int key, bool state) {

    keys[key] = state;
}
