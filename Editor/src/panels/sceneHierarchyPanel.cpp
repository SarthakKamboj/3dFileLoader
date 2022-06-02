#include "sceneHierarchyPanel.h"
#include "imgui.h"
#include "meshRendererSettingsPanel.h"
#include "shaderEditor.h"
#include "shaderRegistry.h"

extern MeshRendererSettingsPanel* meshRenPanelPtr;
extern std::vector<MeshRenderer> meshRenderers;
extern ShaderEditor* shaderEditorPtr;
extern ShaderRegistry* shaderRegistryPtr;

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
			shaderEditorPtr->curShaderProgram = &shaderRegistryPtr->shaders[selectedMeshRenderer.shaderIdx];
			shaderEditorPtr->updateTexturePath();
		}
	}
	ImGui::End();
}