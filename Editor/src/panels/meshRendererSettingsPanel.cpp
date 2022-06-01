#include "meshRendererSettingsPanel.h"
#include "imgui.h"
#include <string>
#include "fbxLoader.h"

extern ImFont* openSansLight;
extern Scene* scenePtr;

void MeshRendererSettingsPanel::render() {
	ImGui::Begin("Mesh Rendering Settings");

	if (!renderSelected) {
		ImGui::Text("Please select an object");
		ImGui::End();
		return;
	}

	Mesh& mesh = curMeshRenderer->mesh;
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
	ImGui::Text("Num children: %i", mesh.numChildren);
	ImGui::PushFont(openSansLight);
	for (int childNumIdx = 0; childNumIdx < mesh.numChildren; childNumIdx++) {
		int childIdx = mesh.childMeshIdxs[childNumIdx];
		ImGui::Text(scenePtr->meshes[childIdx].name.c_str());
	}
	ImGui::PopFont();

	ImGui::Separator();
	ImGui::Checkbox("Display normals", &curMeshRenderer->displayNormals);
	ImGui::Checkbox("Display split normals", &curMeshRenderer->displaySplitNormals);
	ImGui::Checkbox("Wireframe mode", &curMeshRenderer->wireframeMode);

	ImGui::Separator();
	ImGui::Text("Current Shader Program");
	ImGui::Button("Open Shader Editor");

	ImGui::End();
}