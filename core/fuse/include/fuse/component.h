#ifndef COMPONENT_H
#define COMPONENT_H

/** This macro makes it so components are automatically named
	@note Only used for in-engine entity-component tree */
#define super(parent) super(parent, __func__)

namespace fuse {

	struct Entity;

	/** @brief Basic component template */
	struct Component {

		/** @brief Store the name of the component
		  @note Only used for in-engine entity-component tree */
		const char *name = "Component";

		/** @brief Pointer to parent entity */
		Entity *parent = nullptr;

		#pragma push_macro("super")
		#undef super
		/** @brief Sets the parent entity of the component
			@param entity - Pointer to parent entity
			@param name - Name of the component */
		void super(Entity *entity, const char *name);
		#pragma pop_macro("super")

		/** @brief Update component logic */
		virtual void _update() {}
		/** @brief Component render code */
		virtual void _draw() {}

		/** @brief Virtual constructor */
		virtual ~Component() {}
	};

	/** @brief Generate a unique id
		@returns Unique id */
	uint _getUniqueID();

	/** @brief Generate a unique id for each component
		@tparam T - Component that requires an id
		@returns Unique component id
		@note The component much inherit from Component */
	template<typename T>
		uint _getTypeID() {

			static_assert(std::is_base_of<Component, T>::value, "T must inherit from Component");

			static uint id = _getUniqueID();
			return id;
		}

}

#endif

