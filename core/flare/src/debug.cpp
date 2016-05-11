#ifndef NDEBUG
#include "flare/flare.h"

/** @brief Draw Entity-Component node
	@todo Figure out if showChildren is neccesary */
void entityTreeNode(fuse::Entity *entity, bool showChildren = false) {

	if (!entity->isChild() || showChildren) {

		if (ImGui::TreeNode(print::format("%s (%p)", entity->name, entity).c_str())) {

			if (ImGui::TreeNode("Components")) {
				for (fuse::Component *component : entity->components) {

					ImGui::BulletText("%s (%p)", component->name, component);
				}
				ImGui::TreePop();
			}
			if (ImGui::TreeNode("Children")) {
				for (fuse::Entity *child : entity->children) {

					entityTreeNode(child, true);
				}
				ImGui::TreePop();
			}
			ImGui::TreePop();
		}
	}
}

void flare::debug::entityTree() {

	ImGui::Begin("Entities");

	// This is going to be interesting once we introduce multiple 'game states', each game state will have it's own manager
	for (fuse::Entity *entity : *getState()->mainState->manager->getEntities()) {

		entityTreeNode(entity);
	}

	ImGui::End();
}

#endif // NDEBUG
