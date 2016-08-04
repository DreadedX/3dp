#include "fuse/fuse.h"

fuse::Entity::Entity(Manager *manager, Allocator *_allocator, const char *name, fuse::Entity *entity) {

	this->manager = manager;
	this->_allocator = _allocator;
	this->name = name;

	if (entity != nullptr) {

		parent = entity;
		child = true;
	}
}

fuse::Entity::~Entity() {

	for (Entity *child : children) {

		child->kill();
	}

	for (Component *component : components) {

		// delete component;
		allocator::make_delete(*_allocator, *component);
	}
}

bool fuse::Entity::isChild() {

	return child;
}

/** @todo Make a list of all the children, that get killed when the parent gets killed */
fuse::Entity *fuse::Entity::createChild(const char *name) {

	Entity *child = this->manager->createEntity(name, this);
	children.add(child);
	return child;
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
