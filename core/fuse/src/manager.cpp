#include <algorithm>

#include "fuse/manager.h"

void fuse::Manager::init(Allocator *_allocator) {

	manager_allocator = _allocator;
}

void fuse::Manager::update() {

	entities.erase(
			std::remove_if(
				entities.begin(), entities.end(),
				[&](Entity *entity) {

				if (!entity->_isAlive()) {

				// delete entity;
				allocator::make_delete(*manager_allocator, *entity);
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

void fuse::Manager::draw() {

	for (Entity *entity: entities) {

		entity->_draw();
	}
}

/** Create a new entity, add it to the entity list and return a pointer */
fuse::Entity *fuse::Manager::createEntity(const char *name) {

	Entity *entity = new (manager_allocator->allocate(sizeof(Entity), __alignof(Entity))) Entity(this, manager_allocator);
	// Entity *entity = new Entity(this, manager_allocator, name);
	entity->setAttribute("name", name);
	entities.push_back(entity);

	return entity;
}

void fuse::Manager::killAll() {

	for (Entity *entity : entities) {

		entity->kill();
	}

	update();
	entities.clear();
}

std::vector<fuse::Entity*> *fuse::Manager::getEntities() {

	return &entities;
}
