#pragma once

struct FileBrowser {
	FileBrowser();
	void render();
	char* resultBuffer;
	char file[100];
	char curFilePath[200];
	bool open = false;
	int selectedIdx = -1;
};

