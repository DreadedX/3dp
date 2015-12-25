#ifndef NDEBUG
#include "flare/flare.h"

void flare::debug::entityTree() {

    ImGui_ImplGlfwGL3_NewFrame();
    {
	ImGui::Begin("Entities");

	for (fuse::Entity *entity : *fuse::getEntities()) {

	    entityTreeNode(entity);
	}

	ImGui::End();
    }

    ImGui::Render();
}

void flare::debug::entityTreeNode(fuse::Entity *entity, bool showChildren) {

    if (!entity->isChild() || showChildren) {

	if (ImGui::TreeNode(print::format("%s (%p)", entity->name, entity).c_str())) {

	    if (ImGui::TreeNode("Components")) {
		for (fuse::Component *component : entity->components) {

		    ImGui::BulletText("%s (%p)", component->name, component);
		}
		ImGui::TreePop();
	    }
	    if (ImGui::TreeNode("Childeren")) {
		for (fuse::Entity *child : entity->children) {

		    entityTreeNode(child, true);
		}
		ImGui::TreePop();
	    }
	    ImGui::TreePop();
	}
    }
}

#endif // NDEBUG
