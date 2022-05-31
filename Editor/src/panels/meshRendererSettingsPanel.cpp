#include "meshRendererSettingsPanel.h"
#include "imgui.h"
#include <string>

extern ImFont* openSansLight;

void MeshRendererSettingsPanel::render() {
	ImGui::Begin("Mesh Rendering Settings");
	Mesh& mesh = curMeshRenderer.mesh;
	std::string meshNameStr = std::string("Name: ") + mesh.name;
	ImGui::Text(meshNameStr.c_str());
	ImGui::Separator();
	std::string vertexCountStr = std::string("Number of imported vertices: ") + std::to_string(mesh.vertexCount);
	ImGui::Text(vertexCountStr.c_str());
	ImGui::Separator();
	ImGui::Text("Imported Transform");
	glm::vec3& pos = mesh.transform.position;
	glm::vec3& rot = mesh.transform.rotation;
	glm::vec3& scale = mesh.transform.scale;

	ImGui::PushFont(openSansLight);
	ImGui::Text("Position: (%.2f, %.2f, %.2f)", pos.x, pos.y, pos.z);
	ImGui::Text("Rotation: (%.2f, %.2f, %.2f)", rot.x, rot.y, rot.z);
	ImGui::Text("Scale: (%.2f, %.2f, %.2f)", scale.x, scale.y, scale.z);
	ImGui::PopFont();

	ImGui::Separator();
	ImGui::Text("Current Shader Program");
	ImGui::Button("Open Shader Editor");

	ImGui::End();
}