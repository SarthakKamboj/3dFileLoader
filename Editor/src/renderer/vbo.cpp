#include "vbo.h"

VBO::VBO() {
	glGenBuffers(1, &vbo);
}

void VBO::setData(float* vertices, int sizeOfVertices, GLenum usage) {
	bind();
	glBufferData(GL_ARRAY_BUFFER, sizeOfVertices, vertices, usage);
	unbind();
}

void VBO::bind() {
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
}

void VBO::unbind() {
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}