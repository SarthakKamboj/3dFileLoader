#pragma once

#include "glm/glm.hpp"

struct FrameBuffer {
	FrameBuffer();
	void bind();
	void unbind();

	static void ClearBuffers(const glm::vec3& color);

	unsigned int colorTexture;

	unsigned int fbo;
};
