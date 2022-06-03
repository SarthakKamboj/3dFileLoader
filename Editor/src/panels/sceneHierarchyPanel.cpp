#include "sceneHierarchyPanel.h"
#include "imgui.h"
#include "meshRendererSettingsPanel.h"
#include "shaderEditor.h"
#include "shaderRegistry.h"
#include "sceneList.h"

extern MeshRendererSettingsPanel* meshRenPanelPtr;
// extern std::vector<MeshRenderer> meshRenderers;
extern ShaderEditor* shaderEditorPtr;
extern ShaderRegistry* shaderRegistryPtr;
extern SceneList* sceneListPtr;

SceneHierarchyPanel::SceneHierarchyPanel() {
	selectedMeshIdx = -1;
}

void SceneHierarchyPanel::render() {
	ImGui::Begin("Scene Hierarchy");
	int numMeshes = 0;
	if (sceneListPtr->curSceneIdx > -1) {
		numMeshes = sceneListPtr->scenes[sceneListPtr->curSceneIdx].numMeshes;
	}
	for (int meshIdx = 0; meshIdx < numMeshes; meshIdx++) {
		Scene& scene = sceneListPtr->scenes[sceneListPtr->curSceneIdx];
		std::vector<MeshRenderer>& meshRenderers = sceneListPtr->meshRenderLists[sceneListPtr->curSceneIdx];
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