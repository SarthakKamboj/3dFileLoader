#pragma once

#include "panels/panelsManager.h"
#include "renderer/lightFrameBuffer.h"
#include "renderer/shaderProgram.h"
#include "renderer/frameBuffer.h"
#include "renderer/light.h"
#include "primitives/cube.h"

struct SceneRenderer {
	SceneRenderer(Light* light);
	void renderScene();

	FrameBuffer sceneFbo;
	LightFrameBuffer lightFrameBuffer;

private:
	ShaderProgram lightPassShader;
	Cube lightCube;
};
