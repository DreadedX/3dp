#include "demo.h"

void Camera::_update() {

	flare::render::State::Camera *camera = flare::render::getState()->camera;
	GLfloat speed = 50.0f * flare::render::getDeltaTime();

	if (flare::input::keyCheck(GLFW_KEY_W)) {

		camera->position += speed * camera->front;
	}
	if (flare::input::keyCheck(GLFW_KEY_S)) {

		camera->position -= speed * camera->front;
	}
	if (flare::input::keyCheck(GLFW_KEY_A)) {

		camera->position -= glm::normalize(glm::cross(camera->front, camera->up)) * speed;
	}
	if (flare::input::keyCheck(GLFW_KEY_D)) {

		camera->position += glm::normalize(glm::cross(camera->front, camera->up)) * speed;
	}

	// input::Mouse *mouse = input::getMouse();

	if (flare::input::keyCheck(GLFW_KEY_UP)) {

		camera->rotation.y += 10 * speed;
	}
	if (flare::input::keyCheck(GLFW_KEY_DOWN)) {

		camera->rotation.y -= 10 * speed;
	}
	if (flare::input::keyCheck(GLFW_KEY_LEFT)) {

		camera->rotation.x -= 10 * speed;
	}
	if (flare::input::keyCheck(GLFW_KEY_RIGHT)) {

		camera->rotation.x += 10 * speed;
	}

	glm::vec2 delta = glm::vec2(flare::input::getMouse()->position.x - flare::getSettings()->resolution.x/2, flare::getSettings()->resolution.y/2 - flare::input::getMouse()->position.y);
	if (flare::input::keyCheck(GLFW_KEY_SPACE)) {

		camera->rotation.x += delta.x * flare::getSettings()->mouseSensitivity;
		camera->rotation.y += delta.y * flare::getSettings()->mouseSensitivity;
		glfwSetCursorPos(flare::getWindow(), flare::getSettings()->resolution.x/2, flare::getSettings()->resolution.y/2);
	}
}

void JoyCamera::_update() {

	flare::render::State::Camera *camera = flare::render::getState()->camera;
	GLfloat speed = 50.0f * flare::render::getDeltaTime();

	// TODO: Create easier controller support in engine
	int countAxes;
	const float *axes = glfwGetJoystickAxes(GLFW_JOYSTICK_1, &countAxes);

	camera->position -= speed * glm::normalize(glm::vec3(camera->front.x, 0, camera->front.z)) * axes[1];
	camera->position += glm::normalize(glm::cross(camera->front, camera->up)) * speed * axes[0];

	// TODO: Add sensitivity
	camera->rotation.y -= 10 * speed * axes[3];
	camera->rotation.x += 10 * speed * axes[2];

	int countButtons;
	const unsigned char *buttons = glfwGetJoystickButtons(GLFW_JOYSTICK_1, &countButtons);

	camera->position += speed * camera->up * (float)buttons[14];
	camera->position -= speed * camera->up * (float)buttons[13];
}
