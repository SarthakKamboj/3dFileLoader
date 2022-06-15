#pragma once

#include "renderer/texture.h"

struct WorldView {
	WorldView();
	void update();

private:
	Texture testTexture;
};
