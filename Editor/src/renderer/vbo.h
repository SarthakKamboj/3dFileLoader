#pragma once

#include "glad/glad.h"

class VBO {
public:
	VBO();
	void setData(float* vertices, int numVertices, GLenum usage);
	void bind();
	void unbind();

private:
	GLuint vbo;
};
