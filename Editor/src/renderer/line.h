#pragma once

#include "glm/glm.hpp"
#include "shaderProgram.h"
#include "vao.h"
#include "vbo.h"

struct Line_ {
public:
	Line_();
	void setStartPoint(glm::vec3 startPoint);
	void setEndPoint(glm::vec3 endPoint);
	void render();
	void setColor(glm::vec3 color);
	ShaderProgram shaderProgram;

	float vertices[6] = {};
	VAO vao;
	VBO vbo;
};
