#include "vbo.h"

VBO::VBO() {
	glGenBuffers(1, &vbo);
}

void VBO::setData(float* vertices, int byteSizeOfVertices, GLenum usage) {
	bind();
	glBufferData(GL_ARRAY_BUFFER, byteSizeOfVertices, vertices, usage);
	unbind();
}

void VBO::bind() {
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
}

void VBO::unbind() {
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}