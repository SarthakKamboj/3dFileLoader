#include "ebo.h"

EBO::EBO() {
	glGenBuffers(1, &ebo);
}

void EBO::setData(unsigned int* indicies, int sizeOfIndicies, GLenum usage) {
	bind();
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeOfIndicies, indicies, usage);
	unbind();
}

void EBO::bind() {
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
}

void EBO::unbind() {
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}