#ifndef ENTITY_H
#define ENTITY_H

// TODO: Remove this
#include <fuse/fuse.h>

namespace fuse {

    struct Entity {

	std::vector<Component*> components;
	std::array<Component*, MAX_COMPONENTS> componentArray;
	std::bitset<MAX_COMPONENTS> componentBitset;

	std::vector<Entity*> children;

	const char *name = "";

	bool alive = true;
	bool child = false;
	Entity *parent = nullptr;

	Entity(const char *name, Entity *entity);
	~Entity();

	Entity *createChild(const char *name = "Entity");
	bool isChild();
	void kill();
	bool _isAlive();
	void _update();
	void _draw();

	template<typename T>
	bool hasComponent() {
	    if (componentBitset[_getTypeID<T>()]) {
		return true;
	    }
	    return false;
	}

	template<typename T, typename... TArgs>
	T *addComponent(TArgs&&... args) {

	    assert(!hasComponent<T>() /*Entity already has this component*/);

	    T *component = new T(this, std::forward<TArgs>(args)...);

	    components.push_back(component);
	    componentArray[_getTypeID<T>()] = component;
	    componentBitset[_getTypeID<T>()] = true;

	    return component;
	}

	template<typename T>
	T *getComponent() {

	    assert(hasComponent<T>() /*Entity already has this component*/);

	    return reinterpret_cast<T*>(componentArray[_getTypeID<T>()]);
	}
    };
}
#endif

