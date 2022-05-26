#pragma once

#include "glad/glad.h"
#include "vbo.h"

class VAO {
public:
	VAO();
	void attachVBO(VBO& vbo, GLuint index, int numFloatsPerDescriptor, int numFloatsPerVertex, int offsetInBytes);
	void bind();
	void unbind();

private:
	GLuint vao;
};
