#include "shaderEditor.h"
#include "imgui.h"
#include "fileBrowser.h"
#include "helper.h"
#include "panelsManager.h"

extern PanelsManager* g_PanelsManager;

ShaderEditor::ShaderEditor() {
	curShaderProgram = NULL;
	sprintf_s(newTexturePath, "");
}

void ShaderEditor::updateTexturePath() {
	sprintf_s(newTexturePath, curShaderProgram->texture.filePath);
}

void ShaderEditor::update() {
	if (!open) return;
	ImGui::Begin("Shader Editor", &open);

	if (curShaderProgram == NULL) {
		ImGui::Text("Please select a shader to edit");
		ImGui::End();
		return;
	}

	// display shader data
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
		// show texture path
		char texPathStr[300];
		strcpy_s(texPathStr, "Texture Path: ");
		strcat_s(texPathStr, curShaderProgram->texture.filePath);
		ImGui::Text(texPathStr);

		FileBrowser* fileBrowserPtr = &g_PanelsManager->fileBrowser;
		if (ImGui::Button("Update texture")) {
			// open file browser to select new texture image
			int lastIdx = Helper::GetLastIndex(curShaderProgram->texture.filePath, '\\');
			memset(fileBrowserPtr->curFolderPath, 0, 200);
			Helper::CopyBuffer(curShaderProgram->texture.filePath, fileBrowserPtr->curFolderPath, lastIdx);
			fileBrowserPtr->loadMode = FileBrowserLoadMode::IMAGE;
			fileBrowserPtr->resultBuffer = newTexturePath;
			fileBrowserPtr->open = true;
			fileBrowserPtr->validPath = false;
			selectingNewTexturePath = true;
		}
		if (!fileBrowserPtr->open && fileBrowserPtr->validPath && selectingNewTexturePath) {
			// update texture path
			selectingNewTexturePath = false;
			if (!Helper::IsSameString(newTexturePath, curShaderProgram->texture.filePath)) {
				curShaderProgram->texture.updateTextureFilePath(newTexturePath);
			}
		}
	}

	// update object vertex normal displacement
	float prevDisplacement = curShaderProgram->normalDisplacement;
	ImGui::DragFloat("Normal Displacement", &curShaderProgram->normalDisplacement);
	float newDisplacement = curShaderProgram->normalDisplacement;

	if (prevDisplacement != newDisplacement) {
		curShaderProgram->setFloat("displacement", newDisplacement);
	}

	ImGui::End();
}
