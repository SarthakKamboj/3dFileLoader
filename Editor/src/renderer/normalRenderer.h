#pragma once

#include "glm/glm.hpp"
#include "shaderProgram.h"
#include "vao.h"
#include "vbo.h"
#include <vector>

static float i = 0;

struct LinePoints {
	// float vertices[6] = {};
	float start[3] = {};
	float end[3] = {};

	LinePoints();
};

#define NUM_LINES 1000

struct NormalRenderer {
	NormalRenderer();
	void setStartPoint(int idx, glm::vec3 startPoint);
	void setEndPoint(int idx, glm::vec3 endPoint);

	void render(int numLines);
	void setColor(glm::vec3 color);
	ShaderProgram shaderProgram;

	LinePoints lines[NUM_LINES] = {};
	VAO vao;
	VBO vbo;
};