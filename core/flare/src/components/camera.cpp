#include "flare/flare.h"

void flare::component::Camera::_update() {

    render::Camera *camera = render::getCamera();
    GLfloat speed = 5.0f * render::getDeltaTime();

    if (input::keyCheck(GLFW_KEY_W)) {

	camera->posiion += speed * camera->front;
    }
    if (input::keyCheck(GLFW_KEY_S)) {

	camera->posiion -= speed * camera->front;
    }
    if (input::keyCheck(GLFW_KEY_A)) {

	camera->posiion -= glm::normalize(glm::cross(camera->front, camera->up)) * speed;
    }
    if (input::keyCheck(GLFW_KEY_D)) {

	camera->posiion += glm::normalize(glm::cross(camera->front, camera->up)) * speed;
    }

    input::Mouse *mouse = input::getMouse();

    if (input::keyCheck(GLFW_KEY_UP)) {

	mouse->pitch += 10 * speed;
    }
    if (input::keyCheck(GLFW_KEY_DOWN)) {

	mouse->pitch -= 10 * speed;
    }
    if (input::keyCheck(GLFW_KEY_LEFT)) {

	mouse->yaw -= 10 * speed;
    }
    if (input::keyCheck(GLFW_KEY_RIGHT)) {

	mouse->yaw += 10 * speed;
    }

    camera->front.x = cos(glm::radians(mouse->yaw)) * cos(glm::radians(mouse->pitch));
    camera->front.y = sin(glm::radians(mouse->pitch));
    camera->front.z = sin(glm::radians(mouse->yaw)) * cos(glm::radians(mouse->pitch));
    camera->front = glm::normalize(camera->front);
}
