#ifndef MANAGER_H
#define MANAGER_H

#include <vector>

#include "extra/allocator.h"
#include "fuse/entity.h"

namespace fuse {

	class Manager {

		public:

			void init(Allocator *_allocator);

			/** @brief Call the update function of each entity */
			void update();

			/** @brief Call the draw function of each entity */
			void draw();

			/** @brief Create a new entity
			  @param name - Name assigned to the entity */
			Entity *createEntity(const char *name = "Entity");

			/** @brief Kill all entities */
			void killAll();

			/** @brief Get a list of all the entites
			  @returns A pointer to a list containing all entities */
			std::vector<Entity*> *getEntities();

		private:

			/** @brief List of all entities */
			std::vector<fuse::Entity*> entities;

			Allocator *manager_allocator = nullptr;

	};
}

#endif

