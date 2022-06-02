#pragma once

#include "fbxLoader.h"

struct SceneHierarchyPanel {
	// SceneHierarchyPanel(Scene& inScene);
	// SceneHierarchyPanel();
	SceneHierarchyPanel();
	void render();

	int selectedMeshIdx;
	// Scene& scene;
};
