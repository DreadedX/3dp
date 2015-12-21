#ifndef COMPONENT_H
#define COMPONENT_H

// TODO: Remove this
#include <fuse/fuse.h>

namespace fuse {

    struct Entity;

    struct Component {

	Entity *parent = nullptr;

	void super(Entity *entity);

	virtual void _update() {}
	virtual void _draw() {}

	virtual ~Component() {}
    };

    uint _getUniqueID();
    template<typename T>
    uint _getTypeID() {

	static_assert(std::is_base_of<Component, T>::value, "T must inherit from Component");

	static uint id = _getUniqueID();
	return id;
    }

}

#endif

