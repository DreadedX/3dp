#ifndef ENTITY_H
#define ENTITY_H

namespace fuse {

	/** @brief This is really a container for a group of components */
	struct Entity {

		/** @brief List of all the components contained in this entity */
		std::vector<Component*> components;
		/** @brief Predefined array of components for fast lookup */
		std::array<Component*, MAX_COMPONENTS> componentArray;
		/** @brief Bitset used to check if a component is already loaded */
		std::bitset<MAX_COMPONENTS> componentBitset;

		/** @brief List of all child entities of this entity */
		std::vector<Entity*> children;

		/** @brief Entity name */
		const char *name = "";

		/** @brief Is the entity alive */
		bool alive = true;
		/** @brief Is the entity a child of another entity */
		bool child = false;
		/** @brief Parent entity
			@note Only set when the entity is a child*/
		Entity *parent = nullptr;

		/** @brief Entity constructor
			@param name - Entity name
			@param entity - Parent entity, nullptr if this entity is not a child
			@warning You should never manually call this function, use the entity manager to create entities instead */
		Entity(const char *name, Entity *entity);
		/** @brief Entity destructor */
		~Entity();

		/** @brief Create a new entity that is a chil of this entity
			@param name - Child entity name */
		Entity *createChild(const char *name = "Entity");
		/** @brief See if entity is a child entity
			@returns True if entity is a child */
		bool isChild();
		/** @brief Queue entity for removal */
		void kill();
		/** @brief Check if entity is alive
			@returns True if entity is alive */
		bool _isAlive();
		/** @brief Update all component logic in this entity */
		void _update();
		/** @brief Execute draw code for all components in this entity */
		void _draw();

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

				T *component = new T(this, std::forward<TArgs>(args)...);

				components.push_back(component);
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

