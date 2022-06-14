#include "worldView.h"
#include "imgui.h"
#include "glm/glm.hpp"
#include "sceneRenderer.h"

extern SceneRenderer* g_SceneRenderer;
extern int height;

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
		// ImGui::Image((ImTextureID)lightFrameBuffer.colorTexture, actualRenderView, ImVec2(0, 1), ImVec2(1, 0));
		// imgui window is relative from top left and increases going down and to the right
		// opengl is relative from bottom left and increases going up and to the right
		ImVec2 windowPos = ImGui::GetWindowPos();
		ImVec2 windowSize = ImGui::GetWindowSize();

		glm::vec2 openGlPos = glm::vec2(windowPos.x - mainViewport->WorkPos.x, 800 - (windowPos.y - mainViewport->WorkPos.y) - height);
	}

	ImGui::End();
	ImGui::PopStyleVar();


}
