#include "fileBrowser.h"
#include "imgui.h"
#include <string>
#include <iostream>
#include <filesystem>

namespace fs = std::filesystem;

FileBrowser::FileBrowser() {
	memset(curFilePath, 0, 200);
	memset(file, 0, 100);
	resultBuffer = NULL;
}

void FileBrowser::render() {
	if (open) {
		ImGui::Begin("File Browser", &open);
		if (ImGui::Button("Back")) {
			int lastIdx = FileBrowser::GetLastIndex(curFilePath, '\\');
			if (lastIdx != -1) {
				curFilePath[lastIdx] = 0;
			}
		}
		ImGui::Text(curFilePath);

		std::string path(curFilePath);
		static bool once = true;
		int i = 0;
		for (const auto& entry : fs::directory_iterator(path)) {
			std::string filePathStr = entry.path().u8string();
			const char* filePath = filePathStr.c_str();
			int lastIdx = FileBrowser::GetLastIndex(filePath, '\\');
			char buffer[100] = {};
			FileBrowser::CopyBuffer(filePath + lastIdx + 1, buffer, filePathStr.length() - lastIdx);
			if (ImGui::Selectable(buffer, selectedIdx == i)) {
				selectedIdx = i;
				memset(file, 0, 100);
				strcpy_s(file, buffer);
			}
			i++;
		}

		if (ImGui::Button("Select")) {

			if (selectedIdx == -1) {
				if (ImGui::Button("Cancel")) {
					open = false;
				}
				ImGui::End();
				return;
			}

			char newPath[200] = {};
			FileBrowser::CopyBuffer(curFilePath, newPath, 200);
			char slashBuf[2];
			slashBuf[0] = '\\';
			slashBuf[1] = 0;
			FileBrowser::ConcatBuffer(newPath, slashBuf);
			FileBrowser::ConcatBuffer(newPath, file);

			if (fs::is_directory(newPath)) {
				memset(curFilePath, 0, 200);
				FileBrowser::CopyBuffer(newPath, curFilePath, 200);
				selectedIdx = -1;
			}
			else {
				if (!FileBrowser::IsImage(newPath)) {
					if (ImGui::Button("Cancel")) {
						open = false;
					}
					ImGui::End();
					return;
				}
				memset(resultBuffer, 0, 200);
				FileBrowser::ConcatBuffer(resultBuffer, newPath);
				open = false;
			}

		}

		if (ImGui::Button("Cancel")) {
			open = false;
		}
		ImGui::End();
	}
}

int FileBrowser::GetLastIndex(const char* buffer, char c) {
	int res = -1;
	int idx = 0;
	while (*buffer != 0) {
		if (*buffer == c) {
			res = idx;
		}
		buffer++;
		idx++;
	}
	return res;
}

bool FileBrowser::IsImage(const char* filePath) {
	int lastDot = FileBrowser::GetLastIndex(filePath, '.');
	if (lastDot == -1) return false;
	return FileBrowser::IsSameString(filePath + lastDot + 1, "png") || FileBrowser::IsSameString(filePath + lastDot + 1, "jpg");
}

void FileBrowser::CopyBuffer(const char* srcBuffer, char* destBuffer, int size) {
	for (int i = 0; i < size; i++) {
		destBuffer[i] = srcBuffer[i];
	}
}

void FileBrowser::ConcatBuffer(char* buffer, char* extBuffer) {
	while (*buffer != 0) {
		buffer++;
	}
	while (*extBuffer != 0) {
		*buffer = *extBuffer;
		buffer++;
		extBuffer++;
	}
	*buffer = 0;
}

bool FileBrowser::IsSameString(const char* buffer1, const char* buffer2) {
	while (*buffer1 == *buffer2) {
		if (*buffer1 == 0) {
			return true;
		}
		buffer1++;
		buffer2++;
	}
	return false;
}
