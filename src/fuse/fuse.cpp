#include "fuse/fuse.h"

std::vector<fuse::Entity*> entities;

void fuse::update() {

    entities.erase(
	    std::remove_if(
		entities.begin(), entities.end(),
		[](Entity *entity) {

		    if (!entity->_isAlive()) {

			delete entity;

			return true;
		    }

		    return false;
		}
	    ), entities.end()
	);

    for (Entity *entity: entities) {

	entity->_update();
    }
}

void fuse::draw() {

    for (Entity *entity: entities) {

	entity->_draw();
    }
}

// Create a new entity, add it to the entity list and return a pointer
fuse::Entity *fuse::createEntity() {

    Entity *entity = new Entity;
    entities.push_back(entity);

    print::d("Adding entity: '0x%x', entity count: %i", entity, entities.size());

    return entity;
}

// Remove the entity from the entity list and set the used pointer to a nullptr
void fuse::removeEntity(fuse::Entity *entity) {

    std::vector<fuse::Entity*>::iterator location = std::find(entities.begin(), entities.end(), entity);

    if (location != entities.end()) {

	entities.erase(location);

	print::d("Removing entity: '0x%x', entity count: %i", entity, entities.size());
    } else {

	print::w("Tried to remove non-existent entity: '0x%x'", entity);
    }
}
