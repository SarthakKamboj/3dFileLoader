#pragma once

struct MenuBar {
	void update();

	char fbxToLoadPath[300] = {};
	bool selectingFbxToLoad = false;
};
