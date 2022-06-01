#include "fileBrowser.h"
#include "imgui.h"
#include <string>
#include <iostream>
#include <filesystem>
#include "helper.h"

namespace fs = std::filesystem;

extern bool enterPressed;

FileBrowser::FileBrowser() {
	memset(curFilePath, 0, 200);
	memset(file, 0, 100);
	resultBuffer = NULL;
}

void FileBrowser::render() {
	if (open) {
		ImGui::Begin("File Browser", &open);
		if (ImGui::Button("Back")) {
			int lastIdx = Helper::GetLastIndex(curFilePath, '\\');
			if (lastIdx != -1) {
				curFilePath[lastIdx] = 0;
			}
			if (curFilePath[lastIdx - 1] == ':') {
				curFilePath[lastIdx] = '\\';
				curFilePath[lastIdx + 1] = 0;
			}
			selectedIdx = -1;
		}
		ImGui::Text(curFilePath);

		std::string path(curFilePath);
		static bool once = true;
		int i = 0;
		for (const auto& entry : fs::directory_iterator(path)) {
			std::string filePathStr = entry.path().u8string();
			const char* filePath = filePathStr.c_str();
			int lastIdx = Helper::GetLastIndex(filePath, '\\');
			char buffer[100] = {};
			Helper::CopyBuffer(filePath + lastIdx + 1, buffer, filePathStr.length() - lastIdx);
			if (ImGui::Selectable(buffer, selectedIdx == i)) {
				selectedIdx = i;
				memset(file, 0, 100);
				strcpy_s(file, buffer);
			}
			i++;
		}

		if (ImGui::Button("Select") || enterPressed) {

			if (selectedIdx == -1) {
				if (ImGui::Button("Cancel")) {
					open = false;
				}
				ImGui::End();
				return;
			}

			char newPath[200] = {};
			Helper::CopyBuffer(curFilePath, newPath, 200);
			int lastSlash = Helper::GetLastIndex(newPath, '\\');
			if (lastSlash != -1 && newPath[lastSlash + 1] != 0) {
				char slashBuf[2];
				slashBuf[0] = '\\';
				slashBuf[1] = 0;
				Helper::ConcatBuffer(newPath, slashBuf);
			}
			Helper::ConcatBuffer(newPath, file);

			if (fs::is_directory(newPath)) {
				memset(curFilePath, 0, 200);
				Helper::CopyBuffer(newPath, curFilePath, 200);
				selectedIdx = -1;
			}
			else {
				if (!Helper::IsImage(newPath)) {
					if (ImGui::Button("Cancel")) {
						open = false;
					}
					ImGui::End();
					return;
				}
				memset(resultBuffer, 0, 200);
				Helper::ConcatBuffer(resultBuffer, newPath);
				open = false;
			}

		}

		if (ImGui::Button("Cancel")) {
			open = false;
		}
		ImGui::End();
	}
}
