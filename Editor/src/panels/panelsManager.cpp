#include "panelsManager.h"
#include "imgui.h"
#include "sceneRenderer.h"
#include "window.h"

extern int width, height;
extern SceneRenderer* g_SceneRenderer;
extern Window* g_Window;

void setSceneViewWindowConstraint(ImGuiSizeCallbackData* data) {
	data->DesiredSize.y = data->DesiredSize.x * (((float)height) / width);
}

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
	worldView.update();
	lightEditor.update();
}