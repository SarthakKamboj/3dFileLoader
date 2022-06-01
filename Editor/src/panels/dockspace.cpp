#include "dockspace.h"
#include "imgui.h"

void renderDockspace() {
	ImGuiWindowFlags windowFlags = ImGuiWindowFlags_None;

	windowFlags |= ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
	windowFlags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus | ImGuiWindowFlags_NoTitleBar;

	ImGuiViewport* mainViewport = ImGui::GetMainViewport();
	ImGui::SetNextWindowPos(mainViewport->WorkPos);
	ImGui::SetNextWindowSize(ImVec2(mainViewport->WorkSize.x, mainViewport->WorkSize.y));
	ImGui::SetNextWindowViewport(mainViewport->ID);

	ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));

	ImGui::Begin("Dockspace", NULL, windowFlags);

	ImGuiIO& io = ImGui::GetIO();
	ImGuiDockNodeFlags dockspaceFlags = ImGuiDockNodeFlags_None;
	if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable) {
		ImGuiID id = ImGui::GetID("Dock");
		ImGui::DockSpace(id, ImVec2(0, 0), dockspaceFlags);
	}

	ImGui::End();
	ImGui::PopStyleVar(3);
}