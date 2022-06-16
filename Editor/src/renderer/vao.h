#pragma once

#include "glad/glad.h"
#include "vbo.h"

class VAO {
public:
	VAO();
	void attachVBO(const VBO& vbo, GLuint index, int numFloatsPerDescriptor, int stride, int offsetInBytes);
	void bind();
	void unbind();

private:
	GLuint vao;
};
