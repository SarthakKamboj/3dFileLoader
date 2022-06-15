#include "worldView.h"
#include "imgui.h"
#include "glm/glm.hpp"
#include "sceneRenderer.h"

extern SceneRenderer* g_SceneRenderer;
extern int height;

WorldView::WorldView() {
	testTexture = Texture("C:\\Sarthak\\programming\\3dFileLoader\\Editor\\src\\images\\cylinderTexture.png", 0);
}

void WorldView::update() {

	ImGuiWindowFlags worldViewWinFlags = ImGuiWindowFlags_None;

	ImGuiViewport* mainViewport = ImGui::GetMainViewport();
	ImGui::SetNextWindowViewport(mainViewport->ID);

	ImVec2 sceneViewWinPadding(5.0f, 5.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, sceneViewWinPadding);

	// ImGui::SetNextWindowSizeConstraints(ImVec2(100, 100), ImVec2(FLT_MAX, FLT_MAX), setSceneViewWindowConstraint);

	float fontSize = 16.0f;
	ImGui::Begin("World view window", NULL, worldViewWinFlags);
	{
		ImVec2 winSize = ImGui::GetWindowSize();
		ImVec2 actualRenderView(winSize.x - sceneViewWinPadding.x, winSize.y - (2.0f * fontSize) - (2 * sceneViewWinPadding.y));
		ImGui::Image((ImTextureID)g_SceneRenderer->sceneFbo.frameBufferTex, actualRenderView, ImVec2(0, 1), ImVec2(1, 0));
	}

	ImGui::End();
	ImGui::PopStyleVar();


}
