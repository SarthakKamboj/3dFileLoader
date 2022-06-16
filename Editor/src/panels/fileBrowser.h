#pragma once

typedef enum FileBrowserLoadMode {
	IMAGE, SCENE
} FileBrowserLoadMode;

struct FileBrowser {
	FileBrowser();
	void update();
	char* resultBuffer;
	char file[100];
	char curFolderPath[200];
	bool open = false;
	int selectedIdx = -1;
	bool validPath = false;
	FileBrowserLoadMode loadMode;
};

