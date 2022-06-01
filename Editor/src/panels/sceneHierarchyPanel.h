#pragma once

#include "fbxLoader.h"

struct SceneHierarchyPanel {
	SceneHierarchyPanel(Scene& inScene);
	void render();

	int selectedMeshIdx;
	Scene& scene;
};
