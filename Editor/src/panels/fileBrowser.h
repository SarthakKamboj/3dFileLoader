#pragma once

struct FileBrowser {
	FileBrowser();
	void render();
	char* resultBuffer;
	char file[100];
	char curFilePath[200];
	bool open = false;
	int selectedIdx = -1;

	static int GetLastIndex(const char* buffer, char c);
	static void CopyBuffer(const char* srcBuffer, char* destBuffer, int size);
	static void ConcatBuffer(char* buffer, char* extBuffer);
	static bool IsSameString(const char* buffer1, const char* buffer2);
	static bool IsImage(const char* filePath);

};

