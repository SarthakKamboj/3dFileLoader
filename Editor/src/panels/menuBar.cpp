#include "menuBar.h"
#include "renderer/frameBuffer.h"
#include "glad/glad.h"
#include "imgui.h"
#include "panels/panelsManager.h"
#include "helper.h"
#include <filesystem>

namespace fs = std::filesystem;
extern PanelsManager* g_PanelsManager;

void MenuBar::update() {
	FileBrowser& fileBrowser = g_PanelsManager->fileBrowser;
	if (ImGui::BeginMainMenuBar()) {
		if (ImGui::BeginMenu("Menu")) {
			if (ImGui::MenuItem("Open fbx file")) {
				// open filebrowser to select file
				fileBrowser.open = true;
				fileBrowser.validPath = false;
				fileBrowser.resultBuffer = fbxToLoadPath;
				fileBrowser.loadMode = FileBrowserLoadMode::SCENE;
				memset(fileBrowser.curFolderPath, 0, 200);

				// char rootPath[200] = {};
				memset(fileBrowser.curFolderPath, 0, 200);

				fs::path currentPath = fs::current_path();
				std::string curRootStr = currentPath.root_name().string();
				const char* curRootChar = curRootStr.c_str();
				Helper::CopyBuffer(curRootChar, fileBrowser.curFolderPath, Helper::GetLength(curRootChar));
				Helper::ConcatBuffer(fileBrowser.curFolderPath, "\\");

				// Helper::CopyBuffer("C:\\Sarthak\\programming\\3dFileLoader\\Editor\\assets\\3d", fileBrowser.curFolderPath, 200);
				// Helper::CopyBuffer(rootPath, fileBrowser.curFolderPath, 200);

				selectingFbxToLoad = true;
			}

			if (ImGui::MenuItem("Open shader registry")) {
				g_PanelsManager->shaderRegistry.open = true;
			}

			if (ImGui::MenuItem("Open shader editor")) {
				g_PanelsManager->shaderEditor.open = true;
			}

			ImGui::EndMenu();
		}
		ImGui::EndMainMenuBar();
	}

	if (selectingFbxToLoad && !fileBrowser.open && fileBrowser.validPath) {
		selectingFbxToLoad = false;
		g_PanelsManager->sceneList.loadSceneFromFbxFile(fbxToLoadPath);
	}
}