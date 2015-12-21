#include "fuse/fuse.h"

void fuse::Entity::destroy() {

    alive = false;
}

bool fuse::Entity::_isAlive() {

    return alive;
}

void fuse::Entity::_update() {

    for (Component *component : components) {
        
	component->_update();
    }
}

void fuse::Entity::_draw() {

    for (Component *component : components) {
        
	component->_draw();
    }
}
