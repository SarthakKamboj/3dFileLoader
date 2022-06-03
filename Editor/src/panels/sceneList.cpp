#include "sceneList.h"
#include "imgui.h"
#include "fbxLoader.h"
#include <iostream>
#include "helper.h"
#include "panels/meshRendererSettingsPanel.h"

// extern Scene* scenePtr;
// extern std::vector<MeshRenderer> meshRenderers;
extern MeshRendererSettingsPanel* meshRenPanelPtr;

void SceneList::render() {
	ImGui::Begin("Scene List");
	if (curSceneIdx < 0) {
		ImGui::Text("Select a scene");
	}
	else {
		char buffer[200] = {};
		Helper::CopyBuffer("Active scene: ", buffer, 200);
		Helper::ConcatBuffer(buffer, scenes[curSceneIdx].name);
		ImGui::Text(buffer);
	}
	ImGui::Separator();
	for (int i = 0; i < numScenes; i++) {
		if (ImGui::Selectable(scenes[i].name, selectedSceneIdx == i)) {
			selectedSceneIdx = i;
		}
		ImGui::Text("Number of meshes: %i", scenes[i].numMeshes);
	}
	if (numScenes > 0) {
		if (ImGui::Button("Open scene")) {
			if (selectedSceneIdx < 0 || selectedSceneIdx >= numScenes) {
				ImGui::End();
				return;
			}
			// scenePtr = &scenes[selectedSceneIdx];
			curSceneIdx = selectedSceneIdx;
			// meshRenderers = meshRenderLists[selectedSceneIdx];

			resetEditorState();
			selectedSceneIdx = -1;
		}
	}
	else {
		ImGui::Text("Load in a scene");
	}
	ImGui::End();
}

int SceneList::loadSceneFromFbxFile(const char* fbxToLoadPath) {
	Scene scene = loadFbx(fbxToLoadPath);

	if (scene.numMeshes == -1) {
		std::cout << "scene data not valid" << std::endl;
		return -1;
	}

	// meshRenderers.clear();

	std::vector<MeshRenderer> meshRenderers;
	int numMeshes = scene.numMeshes;
	meshRenderers.resize(numMeshes);
	std::vector<Mesh>& meshes = scene.meshes;

	for (int meshId = 0; meshId < numMeshes; meshId++) {
		meshRenderers[meshId] = MeshRenderer(meshes[meshId], meshId);
	}

	int slashIdx = Helper::GetLastIndex(fbxToLoadPath, '\\');
	int dotIdx = Helper::GetLastIndex(fbxToLoadPath, '.');
	memset(scene.name, 0, 150);
	Helper::CopyBuffer(fbxToLoadPath + slashIdx + 1, scene.name, dotIdx - slashIdx - 1);
	numScenes += 1;
	scenes.push_back(scene);
	meshRenderLists.push_back(meshRenderers);
	// scene = sceneList.scenes[sceneList.numScenes - 1];
	curSceneIdx = numScenes - 1;
	resetEditorState();
	return numScenes - 1;
}

void SceneList::resetEditorState() {
	selectedSceneIdx = -1;
	meshRenPanelPtr->curMeshRenderer = NULL;
	meshRenPanelPtr->renderSelected = false;
}
