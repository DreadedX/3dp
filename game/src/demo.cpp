#include "standard.h"

int main() {

    flare::init();

    print::m("This is a demo program for the flare engine!");

    fuse::Entity *player = fuse::createEntity("Player");
    player->addComponent<flare::component::Camera>();

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
