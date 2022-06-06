#pragma once

#include "glm/glm.hpp"

struct FrameBuffer {
	FrameBuffer();
	void bind();
	void unbind();

	static void ClearBuffers(glm::vec3 color);

	unsigned int frameBufferTex;

	unsigned int fbo;
};
