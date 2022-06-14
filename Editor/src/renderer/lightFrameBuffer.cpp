#include "lightFrameBuffer.h"
#include "glad/glad.h"
#include <iostream>
#include "helper.h"
#include "glm/gtc/type_ptr.hpp"

extern int width, height;

LightFrameBuffer::LightFrameBuffer() {
	glGenFramebuffers(1, &fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);

	// glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
	glGenTextures(1, &colorTexture);
	glBindTexture(GL_TEXTURE_2D, colorTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorTexture, 0);

	// glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
	glGenTextures(1, &depthTexture);
	std::cout << "light fbo: " << fbo << " and depth texture: " << depthTexture << std::endl;
	glBindTexture(GL_TEXTURE_2D, depthTexture);
	// glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, width, height, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, NULL);
	// glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, width, height, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, NULL);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
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
	/*
	glm::mat4 lightView(1.0f);
	lightView = glm::translate(lightView, -light->pos);
	return lightView;
	*/
}
