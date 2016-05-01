#ifndef OBJECT_H
#define OBJECT_H

namespace flare {

	namespace component {

		/** @brief Object component, renders a model at the entities position
			@note This component requires a Position component (and optionally a Rotation component) */
		struct Object : fuse::Component {

			/** @brief Pointer to the parent's Position Component */
			Position *position = nullptr;
			/** @brief Pointer to the parent's Rotation Component */
			Rotation *rotation = nullptr;

			/** @brief Pointer to Model Asset */
			asset::Model *model = nullptr;

			/** Draw the object to the screen */
			void _draw() override;

			/** @brief Object constructor
				@param parent - Parent entity
				@param model - Name of the model to load
				@param shader - Name of the shader to load
				@todo It is propably a good idea to remove the shader and just use a global shader */
			Object(fuse::Entity *parent, std::string model) {
				super(parent);

				this->position = parent->getComponent<Position>();
				/** Rotation is optional */
				if (parent->hasComponent<Rotation>()) {

					this->rotation = parent->getComponent<Rotation>();
				}

				this->model = asset::load<asset::Model>(model);
			}
		};
	}
}

#endif

