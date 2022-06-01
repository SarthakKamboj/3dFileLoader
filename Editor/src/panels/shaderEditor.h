#pragma once

#include "renderer/shaderProgram.h"

struct ShaderEditor {
	ShaderEditor();
	void render();

	ShaderProgram* curShaderProgram;
	char newTexturePath[200];
	bool open = false;

	void updateTexturePath();
};
