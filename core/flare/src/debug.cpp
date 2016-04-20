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

	for (fuse::Entity *entity : *fuse::getEntities()) {

		entityTreeNode(entity);
	}

	ImGui::End();
}

#endif // NDEBUG
