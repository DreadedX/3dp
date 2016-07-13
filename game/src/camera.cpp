#include "demo.h"

void Camera::_update() {

	flare::State::Render::Camera *camera = &flare::getState()->render.camera;
	GLfloat speed = 200.0f * flare::render::getDeltaTime();

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

	glm::vec2 delta = glm::vec2(flare::input::getMouse()->position.x - flare::getState()->settings.resolution.x/2, flare::getState()->settings.resolution.y/2 - flare::input::getMouse()->position.y);
	if (flare::input::keyCheck(GLFW_KEY_SPACE)) {

		camera->rotation.x += delta.x * flare::getState()->settings.mouseSensitivity;
		camera->rotation.y += delta.y * flare::getState()->settings.mouseSensitivity;
		glfwSetCursorPos(flare::getState()->window, flare::getState()->settings.resolution.x/2, flare::getState()->settings.resolution.y/2);

		// For some weird reason this is needed for mouse look to work, I have no idea why...
		double x, y;
		glfwGetCursorPos(flare::getState()->window, &x, &y);
	}
}

void JoyCamera::_update() {

	flare::State::Render::Camera *camera = &flare::getState()->render.camera;
	GLfloat speed = 250.0f * flare::render::getDeltaTime();

	// TODO: Create easier controller support in engine
	int countAxes;
	const float *axes = glfwGetJoystickAxes(GLFW_JOYSTICK_2, &countAxes);

	camera->position -= speed * glm::normalize(glm::vec3(camera->front.x, 0, camera->front.z)) * axes[1];
	camera->position += glm::normalize(glm::cross(camera->front, camera->up)) * speed * axes[0];

	// TODO: Add sensitivity
	camera->rotation.y -= 2 * speed * axes[3];
	camera->rotation.x += 2 * speed * axes[2];

	int countButtons;
	const unsigned char *buttons = glfwGetJoystickButtons(GLFW_JOYSTICK_2, &countButtons);

	camera->position += speed/3 * camera->up * (float)buttons[14];
	camera->position -= speed/3 * camera->up * (float)buttons[13];
}
