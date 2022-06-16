#include "panelsManager.h"
#include "imgui.h"
#include "sceneRenderer.h"
#include "window.h"
#include "dockspace.h"

extern SceneRenderer* g_SceneRenderer;
extern Window* g_Window;

void setSceneViewWindowConstraint(ImGuiSizeCallbackData* data) {
	data->DesiredSize.y = data->DesiredSize.x * (((float)Window::height) / Window::width);
}

PanelsManager::PanelsManager() {
	cameraPanel.pov = 45.0f;
	cameraPanel.radius = 2500.0f;
	cameraPanel.angle = 0.0f;
	cameraPanel.yPos = 10.0f;
}

void PanelsManager::update() {

	FrameBuffer::ClearBuffers(glm::vec3(0, 0, 0));
	glViewport(0, 0, Window::width, Window::height);

	renderDockspace();
	meshRenPanel.update();
	sceneHierarchyPanel.update();
	cameraPanel.update();
	shaderEditor.update();
	fileBrowser.update();
	shaderRegistry.update();
	sceneList.update();
	worldView.update();
	lightEditor.update();
	menuBar.update();
}