#pragma once

#include "renderer/meshRenderer.h"

struct MeshRendererSettingsPanel {
	void update();
	MeshRenderer* curMeshRenderer;
	bool renderSelected = false;
};
