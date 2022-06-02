#include "shaderRegistry.h"
#include "imgui.h"

void ShaderRegistry::render() {
	if (open) {
		ImGui::Begin("Shader registry", &open);
		for (int i = 0; i < numShaders; i++) {
			if (ImGui::Selectable(shaders[i]->name, selectedIdx == i)) {
				selectedIdx = i;
			}
		}
		ImGui::End();
	}
}

void ShaderRegistry::addShader(ShaderProgram* shaderPtr) {
	for (int i = 0; i < numShaders; i++) {
		if (shaders[i] == shaderPtr) {
			return;
		}
	}
	shaders.push_back(shaderPtr);
	numShaders += 1;
}
