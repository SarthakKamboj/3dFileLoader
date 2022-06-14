#include "panelsManager.h"

PanelsManager::PanelsManager() {
	cameraPanel.pov = 45.0f;
	cameraPanel.radius = 2500.0f;
	cameraPanel.angle = 0.0f;
	cameraPanel.yPos = 10.0f;
}

void PanelsManager::update() {
	meshRenPanel.render();
	sceneHierarchyPanel.render();
	cameraPanel.render();
	shaderEditor.render();
	fileBrowser.render();
	shaderRegistry.render();
	sceneList.render();
}