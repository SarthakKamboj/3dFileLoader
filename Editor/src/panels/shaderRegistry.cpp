#include "shaderRegistry.h"
#include "imgui.h"
#include "helper.h"
#include "shaderEditor.h"

extern ShaderEditor* shaderEditorPtr;

ShaderRegistry::ShaderRegistry() {
	const char* defaultVert = "C:\\Sarthak\\programming\\3dFileLoader\\Editor\\src\\shaders\\default.vert";
	const char* defaultFrag = "C:\\Sarthak\\programming\\3dFileLoader\\Editor\\src\\shaders\\default.frag";
	defaultShaderProgram = ShaderProgram(defaultVert, defaultFrag);
	memset(defaultShaderProgram.name, 0, 50);
	Helper::CopyBuffer("Default shader", defaultShaderProgram.name, 50);
	shaders.push_back(defaultShaderProgram);
	numShaders++;
}

void ShaderRegistry::render() {
	if (open) {
		ImGui::Begin("Shader registry", &open);
		for (int i = 0; i < numShaders; i++) {
			if (ImGui::Selectable(shaders[i].name, selectedIdx == i)) {
				selectedIdx = i;
				shaderEditorPtr->curShaderProgram = &shaders[i];
			}
		}
		ImGui::End();
	}
}

int ShaderRegistry::addShader(ShaderProgram newShader) {
	sprintf_s(newShader.name, "Shader_%i", numShaders);
	shaders.push_back(newShader);
	numShaders += 1;
	return numShaders - 1;
}
