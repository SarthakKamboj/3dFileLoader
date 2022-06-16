#include "lightFrameBuffer.h"
#include "glad/glad.h"
#include <iostream>
#include "helper.h"
#include "glm/gtc/type_ptr.hpp"
#include "window.h"

LightFrameBuffer::LightFrameBuffer() {
	glGenFramebuffers(1, &fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);

	// create framebuffer depth texture
	// don't need to make color texture since we will just be sampling from depth texture created
	// by the light passes that use the lightframebuffer
	glGenTextures(1, &depthTexture);
	glBindTexture(GL_TEXTURE_2D, depthTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, Window::width, Window::height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthTexture, 0);

	Helper::CheckFrameBufferStatus();

	glBindTexture(GL_TEXTURE_2D, 0);
	glBindBuffer(GL_FRAMEBUFFER, 0);
}

void LightFrameBuffer::bind() {
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
}

void LightFrameBuffer::unbind() {
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

glm::mat4 LightFrameBuffer::getLightViewMat() {
	return glm::lookAt(light->pos, glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
}
