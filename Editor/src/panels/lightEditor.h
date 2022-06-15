#pragma once

#include "renderer/light.h"
#include "renderer/shaderProgram.h"
#include "renderer/vao.h"
#include "renderer/vbo.h"

struct LightEditor {
	LightEditor();
	Light* light;
	bool extraVisible = true;
	void update();
	ShaderProgram depthShader;
	VAO depthVAO;
	VBO depthVBO;
};
