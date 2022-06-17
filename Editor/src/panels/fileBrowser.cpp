#include "fileBrowser.h"
#include "imgui.h"
#include <string>
#include <iostream>
#include <filesystem>
#include "helper.h"
#include "input.h"

namespace fs = std::filesystem;

extern Input* g_Input;

FileBrowser::FileBrowser() {
	memset(curFolderPath, 0, 200);
	memset(file, 0, 100);
	resultBuffer = NULL;
	// std::cout << p << std::endl;
}

void FileBrowser::update() {
	if (open) {
		ImGui::Begin("File Browser", &open);
		if (ImGui::Button("Back")) {
			// cut off part of folder path after the last slash
			int lastIdx = Helper::GetLastIndex(curFolderPath, '\\');
			if (lastIdx != -1) {
				curFolderPath[lastIdx] = 0;
			}
			if (curFolderPath[lastIdx - 1] == ':') {
				curFolderPath[lastIdx] = '\\';
				curFolderPath[lastIdx + 1] = 0;
			}
			selectedIdx = -1;
		}
		ImGui::Text(curFolderPath);
		ImGui::Separator();

		std::string path(curFolderPath);
		static bool once = true;
		int i = 0;
		// iterate through all items in current directory
		for (const auto& entry : fs::directory_iterator(path)) {
			std::string filePathStr = entry.path().u8string();
			const char* filePath = filePathStr.c_str();
			int lastIdx = Helper::GetLastIndex(filePath, '\\');
			char buffer[100] = {};
			// copy name of file or folder into char buffer 
			Helper::CopyBuffer(filePath + lastIdx + 1, buffer, filePathStr.length() - lastIdx);
			if (ImGui::Selectable(buffer, selectedIdx == i)) {
				selectedIdx = i;
				memset(file, 0, 100);
				strcpy_s(file, buffer);
			}
			i++;
		}

		if (ImGui::Button("Select") || g_Input->enterPressed) {

			if (selectedIdx == -1) {
				if (ImGui::Button("Cancel")) {
					open = false;
				}
				ImGui::End();
				return;
			}

			// append the selected file or folder to the current path
			char newPath[200] = {};
			Helper::CopyBuffer(curFolderPath, newPath, 200);
			int lastSlash = Helper::GetLastIndex(newPath, '\\');
			if (lastSlash != -1 && newPath[lastSlash + 1] != 0) {
				char slashBuf[2];
				slashBuf[0] = '\\';
				slashBuf[1] = 0;
				Helper::ConcatBuffer(newPath, slashBuf);
			}
			Helper::ConcatBuffer(newPath, file);

			// if the new path is a directory, view this directory 
			if (fs::is_directory(newPath)) {
				memset(curFolderPath, 0, 200);
				Helper::CopyBuffer(newPath, curFolderPath, 200);
				selectedIdx = -1;
			}
			else {
				// other check if the file is correct
				if (loadMode == FileBrowserLoadMode::IMAGE && !Helper::IsImage(newPath)) {
					if (ImGui::Button("Cancel")) {
						open = false;
					}
					ImGui::End();
					return;
				}
				if (loadMode == FileBrowserLoadMode::SCENE && !Helper::Is3dScene(newPath)) {
					if (ImGui::Button("Cancel")) {
						open = false;
					}
					ImGui::End();
					return;
				}
				memset(resultBuffer, 0, 200);
				Helper::ConcatBuffer(resultBuffer, newPath);
				open = false;
				validPath = true;
			}

		}

		if (ImGui::Button("Cancel")) {
			open = false;
			validPath = false;
		}
		ImGui::End();
	}
}
