#ifndef SPIN_H
#define SPIN_H

#include "fuse/entity.h"
#include "flare/input.h"

struct Spin : fuse::Component {

	GLfloat rotationBase = 0;

	void _update() override {

		static bool toggle = true;
		if (flare::input::keyCheck(GLFW_KEY_F)) {

			if (toggle) {
				toggle = false;
			} else {
				toggle = true;
			}
			flare::input::keySet(GLFW_KEY_F, false);
		}
		if (toggle) {
			parent->setAttribute("rotation", rotationBase + glm::radians(glfwGetTime() * 20.0f));
		}
	}

	Spin(fuse::Entity *parent) {
		super(parent);

		rotationBase = parent->getAttribute<float>("rotation");
	}
};

#endif

