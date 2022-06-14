#include "sceneHierarchyPanel.h"
#include "imgui.h"
#include "meshRendererSettingsPanel.h"
#include "shaderEditor.h"
#include "shaderRegistry.h"
#include "sceneList.h"
#include "panelsManager.h"

// extern MeshRendererSettingsPanel* meshRenPanelPtr;
// extern std::vector<MeshRenderer> meshRenderers;
// extern ShaderEditor* shaderEditorPtr;
// extern ShaderRegistry* shaderRegistryPtr;
// extern SceneList* sceneListPtr;
extern PanelsManager* g_PanelsManager;

SceneHierarchyPanel::SceneHierarchyPanel() {
	selectedMeshIdx = -1;
}

void SceneHierarchyPanel::render() {
	ImGui::Begin("Scene Hierarchy");
	int numMeshes = 0;
	SceneList* sceneListPtr = &g_PanelsManager->sceneList;
	MeshRendererSettingsPanel* meshRenPanelPtr = &g_PanelsManager->meshRenPanel;
	ShaderEditor* shaderEditorPtr = &g_PanelsManager->shaderEditor;
	ShaderRegistry* shaderRegistryPtr = &g_PanelsManager->shaderRegistry;
	if (g_PanelsManager->sceneList.curSceneIdx > -1) {
		numMeshes = sceneListPtr->scenes[sceneListPtr->curSceneIdx].numMeshes;
	}
	for (int meshIdx = 0; meshIdx < numMeshes; meshIdx++) {
		Scene& scene = g_PanelsManager->sceneList.scenes[g_PanelsManager->sceneList.curSceneIdx];
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