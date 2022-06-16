#pragma once

#include "renderer/shaderProgram.h"

#define MAX_NUM_SHADERS 10

struct ShaderRegistry {
	ShaderRegistry();
	int addShader(ShaderProgram& newShader);
	ShaderProgram shaders[MAX_NUM_SHADERS] = {};
	int numShaders = 0;
	int selectedIdx = -1;
	void update();
	bool open = true;
	ShaderProgram defaultShaderProgram;
};
