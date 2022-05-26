#include "vbo.h"

VBO::VBO() {
	glGenBuffers(1, &vbo);
}

void VBO::setData(float* vertices, int numVertices, GLenum usage) {
	glBufferData(GL_ARRAY_BUFFER, numVertices, vertices, usage);
}

void VBO::bind() {
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
}

void VBO::unbind() {
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}