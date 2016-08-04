#include "fuse/fuse.h"

fuse::Entity::Entity(Manager *manager, Allocator *_allocator) {

	this->manager = manager;
	this->_allocator = _allocator;
}

fuse::Entity::~Entity() {

	for (Component *component : components) {

		// delete component;
		allocator::make_delete(*_allocator, *component);
	}
}

void fuse::Entity::kill() {

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

void fuse::Entity::setAttribute(std::string attributeName, std::experimental::any attributeValue) {

	attributes[attributeName] = attributeValue;
}

bool fuse::Entity::hasAttribute(std::string attributeName) {

	return attributes.find(attributeName) != attributes.end();
}
