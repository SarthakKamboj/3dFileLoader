#pragma once

#include "renderer/shaderProgram.h"

class ShaderEditor {
public:
	ShaderEditor();
	void render();

	ShaderProgram curShaderProgram;
};
