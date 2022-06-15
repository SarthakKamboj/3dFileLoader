#pragma once

#include "fbxLoader.h"
#include "renderer/meshRenderer.h"

#define MAX_NUM_SCENES 5
#define MAX_MESHES_PER_SCENE 20

struct SceneList {
	void update();
	Scene scenes[MAX_NUM_SCENES] = {};
	MeshRenderer* meshRenderLists[MAX_NUM_SCENES] = {};
	int numScenes = 0;
	int selectedSceneIdx = -1;
	int curSceneIdx = -1;

	int loadSceneFromFbxFile(const char* filePath);
	void resetEditorState();
};
