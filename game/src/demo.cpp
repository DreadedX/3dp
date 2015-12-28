#include "standard.h"

glm::vec3 cubePositions[] = {
  glm::vec3( 0.0f,  0.0f,  0.0f), 
  glm::vec3( 2.0f,  5.0f, -15.0f), 
  glm::vec3(-1.5f, -2.2f, -2.5f),  
  glm::vec3(-3.8f, -2.0f, -12.3f),  
  glm::vec3( 2.4f, -0.4f, -3.5f),  
  glm::vec3(-1.7f,  3.0f, -7.5f),  
  glm::vec3( 1.3f, -2.0f, -2.5f),  
  glm::vec3( 1.5f,  2.0f, -2.5f), 
  glm::vec3( 1.5f,  0.2f, -1.5f), 
  glm::vec3(-1.3f,  1.0f, -1.5f)  
};

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

int main() {

    flare::init();

    print::m("This is a demo program for the flare engine!");

    fuse::Entity *player = fuse::createEntity("Player");
    player->addComponent<flare::component::Camera>();

#if 0
    for (int i = 0; i < 10; ++i) {

	fuse::Entity *cube = fuse::createEntity("Container");
	cube->addComponent<flare::component::Position>(cubePositions[i]);
	cube->addComponent<flare::component::Rotation>(glm::radians(20.0f * i), glm::vec3(0.5f, 1.0f, 0.0f));
	cube->addComponent<flare::component::Object>("cube", "base/object");
	cube->addComponent<Spin>();
    }
#else
    fuse::Entity *object = fuse::createEntity("Object");
    object->addComponent<flare::component::Position>(glm::vec3(0.0f));
    object->addComponent<flare::component::Rotation>(0, glm::vec3(0.0f, 1.0f, 0.0f));
    object->addComponent<flare::component::Object>("nanosuit", "base/object");
    object->addComponent<Spin>();
#endif

    //fuse::Entity *lamp = fuse::createEntity("Lamp");
    // lamp->addComponent<RotateLight>();

    while (flare::isRunning()) {

	flare::update();

	if (flare::input::keyCheck(GLFW_KEY_X) && player != nullptr) {
	    player->kill();
	    player = nullptr;
	    flare::input::keySet(GLFW_KEY_X, false);

	    print::d("Killing player");
	}
    }

    flare::terminate();
}
