#pragma once

#include "glm/glm.hpp"
#include "shaderProgram.h"
#include "vao.h"
#include "vbo.h"

struct LinePoints {
	float start[3] = {};
	float end[3] = {};
};

#define NUM_LINES 1000

struct NormalRenderer {
	NormalRenderer();
	void setStartPoint(int idx, const glm::vec3& startPoint);
	void setEndPoint(int idx, const glm::vec3& endPoint);

	void render(int numLines);
	void setColor(const glm::vec3& color);
	ShaderProgram shaderProgram;

	LinePoints lines[NUM_LINES] = {};
	VAO vao;
	VBO vbo;
};