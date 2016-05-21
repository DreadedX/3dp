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

void flare::debug::general() {

	ImGui::Begin("General");

	ImGui::Text("Mouse position: %.2f, %.2f", input::getMouse()->position.x, input::getMouse()->position.y);
	ImGui::Text("Yaw/Pitch: %.2f, %.2f", getState()->render.camera.rotation.x, getState()->render.camera.rotation.y);
	ImGui::Text("Camera position: %.2f, %.2f, %.2f", getState()->render.camera.position.x, getState()->render.camera.position.y, getState()->render.camera.position.z);
	ImGui::Text("Memory usage (total): %lu bytes (%i%%)", getState()->mainAllocator->getUsedMemory(), (int)(getState()->mainAllocator->getUsedMemory()*100/getState()->mainAllocator->getSize()));
	ImGui::Text("Memory usage (flux): %lu bytes (%i%%)", getState()->proxyAllocators.flux->getUsedMemory(), (int)(getState()->proxyAllocators.flux->getUsedMemory()*100/getState()->proxyAllocators.flux->getSize()));
	ImGui::Text("Memory usage (fuse): %lu bytes (%i%%)", getState()->proxyAllocators.fuse->getUsedMemory(), (int)(getState()->proxyAllocators.fuse->getUsedMemory()*100/getState()->proxyAllocators.fuse->getSize()));
	ImGui::Text("Memory usage (asset): %lu bytes (%i%%)", getState()->proxyAllocators.asset->getUsedMemory(), (int)(getState()->proxyAllocators.asset->getUsedMemory()*100/getState()->proxyAllocators.asset->getSize()));
	ImGui::Text("Memory usage (model): %lu bytes (%i%%)", getState()->proxyAllocators.model->getUsedMemory(), (int)(getState()->proxyAllocators.model->getUsedMemory()*100/getState()->proxyAllocators.model->getSize()));
	ImGui::Text("Memory usage (entities): %lu bytes (%i%%)", getState()->proxyAllocators.entities->getUsedMemory(), (int)(getState()->proxyAllocators.entities->getUsedMemory()*100/getState()->proxyAllocators.entities->getSize()));

	ImGui::End();
}

#include <map>
struct PlotVarData
{
    ImGuiID        ID;
	std::vector<float>  Data;
    int            DataInsertIdx;
    int            LastFrame;

    PlotVarData() : ID(0), DataInsertIdx(0), LastFrame(-1) {}
};

typedef std::map<ImGuiID, PlotVarData> PlotVarsMap;
static PlotVarsMap  g_PlotVarsMap;

// Plot value over time
// Call with 'value == FLT_MAX' to draw without adding new value to the buffer
void PlotVar(const char* label, float value, float scale_min = FLT_MAX, float scale_max = FLT_MAX, size_t buffer_size = 1200)
{
    assert(label);
    if (buffer_size == 0)
        buffer_size = 120;

    ImGui::PushID(label);
    ImGuiID id = ImGui::GetID("");

    // Lookup O(log N)
    PlotVarData& pvd = g_PlotVarsMap[id];

    // Setup
    if (pvd.Data.capacity() != buffer_size)
    {
        pvd.Data.resize(buffer_size);
        memset(&pvd.Data[0], 0, sizeof(float) * buffer_size);
        pvd.DataInsertIdx = 0;
        pvd.LastFrame = -1;
    }

    // Insert (avoid unnecessary modulo operator)
    if (pvd.DataInsertIdx == buffer_size)
        pvd.DataInsertIdx = 0;
    int display_idx = pvd.DataInsertIdx;
    if (value != FLT_MAX)
        pvd.Data[pvd.DataInsertIdx++] = value;

    // Draw
    int current_frame = ImGui::GetFrameCount();
    if (pvd.LastFrame != current_frame)
    {
        ImGui::PlotLines("##plot", &pvd.Data[0], buffer_size, pvd.DataInsertIdx, NULL, scale_min, scale_max, ImVec2(0, 40));
        ImGui::SameLine();
        ImGui::Text("%s\n%-3.4f", label, pvd.Data[display_idx]);    // Display last value in buffer
        pvd.LastFrame = current_frame;
    }

    ImGui::PopID();
}

void PlotVarFlushOldEntries()
{
    int current_frame = ImGui::GetFrameCount();
    for (PlotVarsMap::iterator it = g_PlotVarsMap.begin(); it != g_PlotVarsMap.end(); )
    {
        PlotVarData& pvd = it->second;
        if (pvd.LastFrame < current_frame - fmax(400,(int)pvd.Data.size()))
            it = g_PlotVarsMap.erase(it);
        else
            ++it;
    }
}

/** @todo This should have some sort of way to view what took how long each frame */
void flare::debug::timers() {

	ImGui::Begin("Rendering");

	PlotVar("Delta time", getState()->render.deltaTime * 1000);
	for (Timer timer : getState()->timers) {

		PlotVar(timer.name.c_str(), timer.time * 1000);
	}

	getState()->timers.clear();

	ImGui::End();
}

void flare::debug::debug() {

	static bool showDebug = false;
	if (input::keyCheck(GLFW_KEY_F3)) {
		showDebug = !showDebug;
		input::keySet(GLFW_KEY_F3, false);
	}
	if (showDebug) {
		ImGui_ImplGlfwGL3_NewFrame();
		{
			entityTree();
			general();
			timers();
		}
		ImGui::Render();
	}
}

#endif // NDEBUG
