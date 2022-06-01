#include "shaderEditor.h"
#include "imgui.h"

ShaderEditor::ShaderEditor() {
	curShaderProgram = NULL;
}

bool isCharSame(const char* buffer1, const char* buffer2) {
	while (*buffer1 == *buffer2) {
		if (*buffer1 == 0) {
			return true;
		}
		buffer1++;
		buffer2++;
	}
	return false;
}


void ShaderEditor::updateTexturePath() {
	sprintf_s(newTexturePath, curShaderProgram->texture.filePath);
}

void ShaderEditor::render() {
	// ImGui::ShowDemoWindow();
	if (!open) return;
	ImGui::Begin("Shader Editor", &open);

	if (curShaderProgram == NULL) {
		ImGui::Text("Please select a shader to edit");
		ImGui::End();
		return;
	}

	ImGui::InputText("Name", curShaderProgram->name, 50);

	glm::vec3 prevColor = curShaderProgram->color;
	ImGui::ColorPicker3("Color", &curShaderProgram->color.x);
	glm::vec3 newColor = curShaderProgram->color;

	if (prevColor != newColor) {
		curShaderProgram->setVec3("color", newColor);
	}

	bool prevTexBasedColor = curShaderProgram->textureBasedColor;
	ImGui::Checkbox("Texture Based Coloring", &curShaderProgram->textureBasedColor);
	bool newTexBasedColor = curShaderProgram->textureBasedColor;

	if (prevTexBasedColor != newTexBasedColor) {
		curShaderProgram->setInt("renderTexture", newTexBasedColor);
	}

	if (curShaderProgram->textureBasedColor) {
		ImGui::InputText("Texture File Path", newTexturePath, 200);
		if (ImGui::Button("Update texture") && !isCharSame(curShaderProgram->texture.filePath, newTexturePath)) {
			curShaderProgram->texture.updateTextureFilePath(newTexturePath);
		}
	}


	float prevDisplacement = curShaderProgram->normalDisplacement;
	ImGui::DragFloat("Normal Displacement", &curShaderProgram->normalDisplacement);
	float newDisplacement = curShaderProgram->normalDisplacement;

	if (prevDisplacement != newDisplacement) {
		curShaderProgram->setFloat("displacement", newDisplacement);
	}

	ImGui::End();
}
