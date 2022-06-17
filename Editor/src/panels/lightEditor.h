#pragma once

#include "renderer/light.h"
#include "renderer/shaderProgram.h"
#include "renderer/vao.h"
#include "renderer/vbo.h"
#include "renderer/texture.h"

struct LightEditor {
	LightEditor();
	Light* light;
	bool extraVisible = true;
	void update();
	void render();
	ShaderProgram depthShader;
	VAO depthVAO;
	VBO depthVBO;

private:
	glm::vec4 openGlViewportLightWin;
	Texture testTexture;
};
