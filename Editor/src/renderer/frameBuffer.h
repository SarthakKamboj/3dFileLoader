#pragma once

class FrameBuffer {
public:
	FrameBuffer();
	void bind();
	void unbind();

	static void ClearBuffers();

	unsigned int frameBufferTex;

private:
	unsigned int fbo;
};
