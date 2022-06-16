#pragma once

#include "glad/glad.h"

class VBO {
public:
	VBO();
	void setData(float* vertices, int byteSizeOfVertices, GLenum usage);
	void bind() const;
	void unbind() const;

private:
	GLuint vbo;
};
