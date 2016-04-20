#include "fuse/fuse.h"

/** @brief List of all entities */
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

/** Create a new entity, add it to the entity list and return a pointer */
fuse::Entity *fuse::createEntity(const char *name, Entity *parent) {

    Entity *entity = new Entity(name, parent);
    entities.push_back(entity);

    return entity;
}

void fuse::killAll() {
    
    for (Entity *entity : entities) {

	entity->kill();
    }
}

std::vector<fuse::Entity*> *fuse::getEntities() {

    return &entities;
}
