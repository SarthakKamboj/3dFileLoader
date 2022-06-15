#pragma once

#include "renderer/shaderProgram.h"

struct ShaderEditor {
	ShaderEditor();
	void update();

	ShaderProgram* curShaderProgram;
	char newTexturePath[200];
	bool open = false;
	bool selectingNewTexturePath = false;

	void updateTexturePath();
};
