#include "shaderRegistry.h"
#include "imgui.h"
#include "helper.h"
#include "shaderEditor.h"
#include "panelsManager.h"

extern PanelsManager* g_PanelsManager;

ShaderRegistry::ShaderRegistry() {
	// add default shader to registry
	const char* defaultVert = "C:\\Sarthak\\programming\\3dFileLoader\\Editor\\src\\shaders\\default.vert";
	const char* defaultFrag = "C:\\Sarthak\\programming\\3dFileLoader\\Editor\\src\\shaders\\default.frag";
	defaultShaderProgram = ShaderProgram(defaultVert, defaultFrag);
	memset(defaultShaderProgram.name, 0, 50);
	Helper::CopyBuffer("Default shader", defaultShaderProgram.name, 50);
	shaders[0] = defaultShaderProgram;
	defaultShaderProgram.setVec3("color", glm::vec3(1, 1, 1));
	defaultShaderProgram.setInt("useSplit", 1);
	numShaders++;
}

void ShaderRegistry::update() {
	if (open) {
		// show all shaders
		ImGui::Begin("Shader registry", &open);
		for (int i = 0; i < numShaders; i++) {
			if (ImGui::Selectable(shaders[i].name, selectedIdx == i)) {
				selectedIdx = i;
				g_PanelsManager->shaderEditor.curShaderProgram = &shaders[i];
			}
		}
		ImGui::End();
	}
}

int ShaderRegistry::addShader(ShaderProgram& newShader) {
	if (numShaders == MAX_NUM_SHADERS) return -1;
	sprintf_s(newShader.name, "Shader_%i", numShaders);
	shaders[numShaders] = newShader;
	numShaders += 1;
	return numShaders - 1;
}
