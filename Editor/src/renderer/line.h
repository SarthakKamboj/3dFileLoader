#pragma once

#include "glm/glm.hpp"
#include "shaderProgram.h"
#include "vao.h"
#include "vbo.h"

class Line {
public:
	Line();
	void setStartPoint(glm::vec3 startPoint);
	void setEndPoint(glm::vec3 endPoint);
	void render();
	void setColor(glm::vec3 color);
	ShaderProgram shaderProgram;
private:
	float vertices[6] = {};
	VAO vao;
	VBO vbo;
};
