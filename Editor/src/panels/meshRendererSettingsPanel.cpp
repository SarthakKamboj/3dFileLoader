#include "meshRendererSettingsPanel.h"
#include "imgui.h"
#include <string>
#include "fbxLoader.h"
#include "panels/shaderEditor.h"
#include "panels/shaderRegistry.h"
#include "helper.h"
#include "panels/sceneList.h"

extern ImFont* openSansLight;
// extern Scene* scenePtr;
extern ShaderEditor* shaderEditorPtr;
extern ShaderRegistry* shaderRegistryPtr;
extern SceneList* sceneListPtr;

void MeshRendererSettingsPanel::render() {
	ImGui::ShowDemoWindow();
	ImGui::Begin("Mesh Rendering Settings");

	if (sceneListPtr->curSceneIdx < 0) {
		ImGui::Text("Please select a scene");
		ImGui::End();
		return;
	}

	if (!renderSelected) {
		ImGui::Text("Please select an object");
		ImGui::End();
		return;
	}

	Scene* scenePtr = &sceneListPtr->scenes[sceneListPtr->curSceneIdx];
	Mesh* mesh = &scenePtr->meshes[curMeshRenderer->meshIdx];
	std::string meshNameStr = std::string("Name: ") + mesh->name;
	ImGui::Text(meshNameStr.c_str());
	ImGui::Separator();
	std::string vertexCountStr = std::string("Number of imported vertices: ") + std::to_string(mesh->vertexCount);
	ImGui::Text(vertexCountStr.c_str());
	ImGui::Separator();
	ImGui::Text("Imported Transform");
	glm::vec3& pos = mesh->transform.position;
	glm::vec3& rot = mesh->transform.rotation;
	glm::vec3& scale = mesh->transform.scale;

	ImGui::PushFont(openSansLight);
	ImGui::DragFloat3("Position: ", &pos.x, 0.1);
	ImGui::DragFloat3("Rotation: ", &rot.x, 1, -180, 180);
	ImGui::DragFloat3("Scale:", &scale.x, 0.1, 0, 10);
	ImGui::PopFont();

	ImGui::Separator();
	ImGui::Text("Num children: %i", mesh->numChildren);
	ImGui::PushFont(openSansLight);
	for (int childNumIdx = 0; childNumIdx < mesh->numChildren; childNumIdx++) {
		int childIdx = mesh->childMeshIdxs[childNumIdx];
		ImGui::Text(scenePtr->meshes[childIdx].name.c_str());
	}
	ImGui::PopFont();

	ImGui::Separator();
	ImGui::Checkbox("Display normals", &curMeshRenderer->displayNormals);
	ImGui::Checkbox("Display split normals", &curMeshRenderer->displaySplitNormals);
	ImGui::Checkbox("Wireframe mode", &curMeshRenderer->wireframeMode);

	ImGui::Separator();
	char shaderProgText[200];
	Helper::CopyBuffer("Current Shader Program: ", shaderProgText, 200);
	Helper::ConcatBuffer(shaderProgText, shaderRegistryPtr->shaders[curMeshRenderer->shaderIdx].name);
	ImGui::Text(shaderProgText);

	char options[400] = {};
	char buffer[1];
	char* runningBuffer = options;
	for (int i = 0; i < shaderRegistryPtr->numShaders; i++) {
		Helper::ConcatBuffer(runningBuffer, shaderRegistryPtr->shaders[i].name);
		while (*runningBuffer != 0) {
			runningBuffer++;
		}
		runningBuffer++;
	}
	ImGui::Combo("Attached shader", &curMeshRenderer->shaderIdx, options);
	ImGui::Checkbox("Smooth shading", &curMeshRenderer->useNormals);

	ImGui::Separator();
	if (ImGui::Button("Open Shader Editor")) {
		shaderEditorPtr->open = true;
		shaderEditorPtr->curShaderProgram = &shaderRegistryPtr->shaders[curMeshRenderer->shaderIdx];
		shaderEditorPtr->updateTexturePath();
	}



	if (ImGui::Button("Create new shader")) {
		const char* defaultVert = "C:\\Sarthak\\programming\\3dFileLoader\\Editor\\src\\shaders\\default.vert";
		const char* defaultFrag = "C:\\Sarthak\\programming\\3dFileLoader\\Editor\\src\\shaders\\default.frag";
		ShaderProgram shaderProgram(defaultVert, defaultFrag);
		shaderProgram.setInt("renderTexture", 0);
		shaderProgram.setInt("material.diffuse", 0);
		shaderProgram.setVec3("color", glm::vec3(0, 1, 0));
		int newShaderIdx = shaderRegistryPtr->addShader(shaderProgram);
		curMeshRenderer->shaderIdx = newShaderIdx;
		shaderEditorPtr->open = true;
		shaderEditorPtr->curShaderProgram = &shaderRegistryPtr->shaders[curMeshRenderer->shaderIdx];
	}

	ImGui::End();
}