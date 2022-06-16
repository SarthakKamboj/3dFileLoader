#include "panelsManager.h"
#include "imgui.h"
#include "sceneRenderer.h"
#include "window.h"
#include "dockspace.h"
#include "imgui_impl_opengl3.h"

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
	shaderRegistry.update();
	sceneList.update();
	worldView.update();
	fileBrowser.update();
	menuBar.update();
	lightEditor.update();
}

void PanelsManager::render() {
	glViewport(0, 0, (int)g_Window->ioPtr->DisplaySize.x, (int)g_Window->ioPtr->DisplaySize.y);
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

	lightEditor.render();
}