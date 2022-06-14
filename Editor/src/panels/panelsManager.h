#pragma once

#include "panels/meshRendererSettingsPanel.h"
#include "panels/shaderEditor.h"
#include "panels/fileBrowser.h"
#include "panels/shaderRegistry.h"
#include "panels/sceneList.h"
#include "panels/cameraPanel.h"
#include "panels/sceneHierarchyPanel.h"
#include "panels/worldView.h"
#include "panels/lightEditor.h"

struct PanelsManager {
	PanelsManager();
	void update();

	MeshRendererSettingsPanel meshRenPanel;
	ShaderEditor shaderEditor;
	FileBrowser fileBrowser;
	ShaderRegistry shaderRegistry;
	SceneList sceneList;
	CameraPanel cameraPanel;
	SceneHierarchyPanel sceneHierarchyPanel;
	LightEditor lightEditor;
	WorldView worldView;
};