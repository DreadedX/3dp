#ifndef ENTITY_H
#define ENTITY_H

#include <experimental/any>
#include <bitset>
#include <unordered_map>

#include "extra/array.h"
#include "extra/allocator.h"

#include "fuse/component.h"

namespace fuse {

	struct Manager;

	/** @brief This is really a container for a group of components */
	struct Entity {

		/** @brief List of all the components contained in this entity */
		Array<Component*> components;
		/** @brief Storage for entity attributes */
		std::unordered_map<std::string, std::experimental::any> attributes;
		/** @brief Predefined array of components for fast lookup */
		std::array<Component*, MAX_COMPONENTS> componentArray;
		/** @brief Bitset used to check if a component is already loaded */
		std::bitset<MAX_COMPONENTS> componentBitset;

		/** @brief Is the entity alive */
		bool alive = true;

		Manager *manager = nullptr;

		/** @brief Entity constructor
			@param name - Entity name
			@warning You should never manually call this function, use the entity manager to create entities instead */
		Entity(Manager *manager, Allocator *_allocator);
		/** @brief Entity destructor */
		~Entity();

		Allocator *_allocator;

		/** @brief Queue entity for removal */
		void kill();
		/** @brief Check if entity is alive
			@returns True if entity is alive */
		bool _isAlive();
		/** @brief Update all component logic in this entity */
		void _update();
		/** @brief Execute draw code for all components in this entity */
		void _draw();

		void setAttribute(std::string attributeName, std::experimental::any attributeValue);

		bool hasAttribute(std::string attributeName);

		template<typename T>
		T getAttribute(const char *attributeName) {

			assert(attributes.find(attributeName) != attributes.end());

			return std::experimental::any_cast<T>(attributes[attributeName]);
		}

		/** @brief Check if the entity has a certain component
			@tparam T - Component type to check for
			@returns True if component exists */
		template<typename T>
			bool hasComponent() {
				if (componentBitset[_getTypeID<T>()]) {
					return true;
				}
				return false;
			}

		/** @brief Add new component to the entity
			@tparam T - Component type to add
			@returns Pointer to newly add component
			@note A component can only be added once */
		template<typename T, typename... TArgs>
			T *addComponent(TArgs&&... args) {

				assert(!hasComponent<T>() /*Entity already has this component*/);

				T *component = new (_allocator->allocate(sizeof(T), __alignof(T))) T(this, std::forward<TArgs>(args)...);

				components.add(component);
				componentArray[_getTypeID<T>()] = component;
				componentBitset[_getTypeID<T>()] = true;

				return component;
			}

		/** @brief Get a pointer to a certain component type in the entity
			@tparam T - Component type that you want a pointer to
			@returns Pointer to requested component */
		template<typename T>
			T *getComponent() {

				assert(hasComponent<T>() /*Entity already has this component*/);

				return reinterpret_cast<T*>(componentArray[_getTypeID<T>()]);
			}
	};
}
#endif

