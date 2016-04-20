#ifndef MANAGER_H
#define MANAGER_H

namespace fuse {

	/** @brief Call the update function of each entity */
    void update();

	/** @brief Call the draw function of each entity */
    void draw();

	/** @brief Create a new entity
		@param name - Name assigned to the entity
		@param parent - Parent of the entity, nullptr if the entity has no parent
		@warning Never call this function with a parent, use Entity::addChild instead */
    Entity *createEntity(const char *name = "Entity", Entity *parent = nullptr);

	/** @brief Kill all entities */
    void killAll();

	/** @brief Get a list of all the entites
		@returns A pointer to a list containing all entities */
    std::vector<Entity *> *getEntities();
}

#endif

