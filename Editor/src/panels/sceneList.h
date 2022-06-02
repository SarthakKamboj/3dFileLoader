#pragma once

#include <vector>
#include "fbxLoader.h"
#include "renderer/meshRenderer.h"

struct SceneList {
	void render();
	std::vector<Scene> scenes;
	std::vector<std::vector<MeshRenderer>> meshRenderLists;
	int numScenes = 0;
	int selectedSceneIdx = -1;
	int curSceneIdx = -1;

	int loadSceneFromFbxFile(const char* filePath);
};
