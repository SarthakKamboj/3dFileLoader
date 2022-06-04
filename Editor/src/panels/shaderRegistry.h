#pragma once

#include "renderer/shaderProgram.h"
#include <vector>

struct ShaderRegistry {
	ShaderRegistry();
	int addShader(ShaderProgram newShader);
	std::vector<ShaderProgram> shaders;
	int numShaders = 0;
	int selectedIdx = -1;
	void render();
	bool open = false;
	ShaderProgram defaultShaderProgram;
};
