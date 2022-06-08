#pragma once

#include "light.h"

struct LightFrameBuffer {
	LightFrameBuffer();
	void bind();
	void unbind();

	Light* light = NULL;
	unsigned int fbo;
	unsigned int depthTexture;
	unsigned int colorTexture;

	glm::mat4 getLightViewMat();
};
