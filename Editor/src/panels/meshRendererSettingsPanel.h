#pragma once

#include "renderer/meshRenderer.h"

struct MeshRendererSettingsPanel {
	void render();
	MeshRenderer* curMeshRenderer;
	bool renderSelected = false;
};
