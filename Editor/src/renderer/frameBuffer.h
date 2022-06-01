#pragma once

#include "glm/glm.hpp"

class FrameBuffer {
public:
	FrameBuffer();
	void bind();
	void unbind();

	static void ClearBuffers(glm::vec3 color);

	unsigned int frameBufferTex;

private:
	unsigned int fbo;
};
