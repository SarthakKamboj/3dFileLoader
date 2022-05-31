#pragma once

#include "fbxLoader.h"

class SceneHierarchyPanel {
public:
	SceneHierarchyPanel(Scene& inScene);
	void render();

private:
	int selectedMeshIdx;
	Scene& scene;
};
