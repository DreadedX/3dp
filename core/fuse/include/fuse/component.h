#ifndef COMPONENT_H
#define COMPONENT_H

// NOTE: Components automatically get named
#define super(parent) super(parent, __func__)

namespace fuse {

    struct Entity;

    struct Component {

	const char *name = "Component";

	Entity *parent = nullptr;

	#pragma push_macro("super")
	#undef super
	void super(Entity *entity, const char *name);
	#pragma pop_macro("super")

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

