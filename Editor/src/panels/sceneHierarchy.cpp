#include "sceneHierarchyPanel.h"
#include "imgui.h"
#include "meshRendererSettingsPanel.h"

extern MeshRendererSettingsPanel* meshRenPanelPtr;
extern std::vector<MeshRenderer> meshRenderers;

SceneHierarchyPanel::SceneHierarchyPanel(Scene& inScene) : scene(inScene) {
	selectedMeshIdx = -1;
}

void SceneHierarchyPanel::render() {
	ImGui::Begin("Scene Hierarchy");
	for (int meshIdx = 0; meshIdx < scene.numMeshes; meshIdx++) {
		if (ImGui::Selectable(scene.meshes[meshIdx].name.c_str(), meshIdx == selectedMeshIdx)) {
			selectedMeshIdx = meshIdx;
			MeshRenderer& selectedMeshRenderer = meshRenderers[selectedMeshIdx];
			meshRenPanelPtr->curMeshRenderer = &selectedMeshRenderer;
			meshRenPanelPtr->renderSelected = true;
		}
	}
	ImGui::End();
}