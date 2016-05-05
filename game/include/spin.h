#ifndef SPIN_H
#define SPIN_H

struct Spin : fuse::Component {

	flare::component::Rotation *rotation = nullptr;
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
			rotation->rotation = rotationBase + glm::radians(glfwGetTime() * 20.0f);
		}
	}

	Spin(fuse::Entity *parent) {
		super(parent);

		rotation = parent->getComponent<flare::component::Rotation>();
		rotationBase = rotation->rotation;
	}
};

#endif

