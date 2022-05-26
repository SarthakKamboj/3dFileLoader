#include "vao.h"

VAO::VAO() {
	glGenVertexArrays(1, &vao);
}

void VAO::attachVBO(VBO& vbo, GLuint index, int numFloatsPerVertex, int offsetInBytes) {
	glEnableVertexAttribArray(index);
	vbo.bind();
	glVertexAttribPointer(index, numFloatsPerVertex, GL_FLOAT, GL_FALSE, numFloatsPerVertex * sizeof(float), (void*)offsetInBytes);
	vbo.unbind();
}

void VAO::bind() {
	glBindVertexArray(vao);
}

void VAO::unbind() {
	glBindVertexArray(0);
}