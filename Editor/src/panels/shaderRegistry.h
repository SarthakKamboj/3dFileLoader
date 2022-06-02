#pragma once

#include "renderer/shaderProgram.h"
#include <vector>

struct ShaderRegistry {
	void addShader(ShaderProgram* shaderPtr);
	std::vector<ShaderProgram*> shaders;
	int numShaders = 0;
	int selectedIdx = -1;
	void render();
	bool open = true;
};
