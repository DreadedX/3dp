#include "standard.h"

int main() {

    flare::init();

    print::m("This is a demo program for the flare engine!");

    // Creating a new entity test
    fuse::Entity *entity1 = fuse::createEntity();
    fuse::Entity *entity2 = fuse::createEntity();

    fuse::Entity *entity3 = fuse::createEntity();

    // Removing entities test
    fuse::removeEntity(entity1);
    fuse::removeEntity(entity1);
    fuse::removeEntity(entity2);

    struct OtherComponent : fuse::Component {

	OtherComponent(fuse::Entity *entity) {
	    super(entity);
	}

	void _update() override {

	    print::m("TEST");
	}
    };

    // This is just a test component for the entity system
    // Also show components can add other components to their parent
    struct TestComponent : fuse::Component {

	TestComponent(fuse::Entity *entity) {
	    super(entity);
	    if (!parent->hasComponent<OtherComponent>()) {

		parent->addComponent<OtherComponent>();
	    }
	    print::m("Parent: 0x%x", parent);
	}

	void _update() override {

	    print::m("Update", parent);
	}

	void _draw() override {

	    print::m("Draw", parent);
	}

    };


    entity3->addComponent<TestComponent>();

    while (flare::isRunning()) {

	flare::update();
    }

    flare::terminate();
}
