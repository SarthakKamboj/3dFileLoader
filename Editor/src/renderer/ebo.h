#pragma once

#include "glad/glad.h"

class EBO {
public:
	EBO();
	void setData(unsigned int* indicies, int sizeOfIndicies, GLenum usage);
	void bind();
	void unbind();

private:
	GLuint ebo;
};
