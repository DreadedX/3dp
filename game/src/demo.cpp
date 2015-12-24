#include "standard.h"

int main() {

    flare::init();

    print::m("This is a demo program for the flare engine!");

    struct PlayerComponent : fuse::Component {
	
	PlayerComponent(fuse::Entity *parent) {
	    super(parent);
	}
    };
    struct WeaponComponent : fuse::Component {
	
	WeaponComponent(fuse::Entity *parent) {
	    super(parent);
	}
    };

    fuse::Entity *player = fuse::createEntity("Player");
    player->addComponent<PlayerComponent>();
    fuse::Entity *weapon = player->createChild("Weapon");
    weapon->addComponent<WeaponComponent>();

    fuse::createEntity("Enemy");
    fuse::createEntity("Enemy");
    fuse::createEntity();

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
