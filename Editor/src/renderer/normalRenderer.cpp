#include "normalRenderer.h"


LinePoints::LinePoints() {
	i += 1;
}

NormalRenderer::NormalRenderer() {

	lines[0].end[0] = 0;
	lines[0].end[1] = 10000;
	lines[0].end[2] = 0;

	vao.bind();
	vbo.bind();
	vbo.setData((float*)&lines[0], sizeof(LinePoints) * NUM_LINES, GL_DYNAMIC_DRAW);
	vao.attachVBO(vbo, 0, 3, 3 * sizeof(float), 0);
	vao.unbind();
	vbo.unbind();

	const char* lineVert = "C:\\Sarthak\\programming\\3dFileLoader\\Editor\\src\\shaders\\line.vert";
	const char* lineFrag = "C:\\Sarthak\\programming\\3dFileLoader\\Editor\\src\\shaders\\line.frag";

	shaderProgram = ShaderProgram(lineVert, lineFrag);

	setColor(glm::vec3(0, 0, 1));
}

void NormalRenderer::setColor(glm::vec3 color) {
	shaderProgram.setVec3("color", color);
}

void NormalRenderer::render(int numLines) {
	// vbo.setData((float*)&lines[0], sizeof(LinePoints) * NUM_LINES, GL_DYNAMIC_DRAW);
	shaderProgram.bind();
	vao.bind();
	// glDrawArrays(GL_LINES, 0, numLines * 2);
	glDrawArrays(GL_LINES, 0, numLines * 2);
	// glDrawArrays(GL_LINES, 0, 2);
	// glDrawArrays(GL_LINES, 0, NUM_LINES * 2);
	vao.unbind();
	shaderProgram.unbind();
}

void NormalRenderer::setStartPoint(int idx, glm::vec3 startPoint) {

	if (idx >= NUM_LINES) return;

	lines[idx].start[0] = startPoint.x;
	lines[idx].start[1] = startPoint.y;
	lines[idx].start[2] = startPoint.z;

	/*
	float* vertices = lines[idx].start;
	vertices[0] = startPoint.x;
	vertices[1] = startPoint.y;
	vertices[2] = startPoint.z;
	*/
	// vertices[0] = 0;
	// vertices[1] = 0;
	// vertices[2] = 0;
	vbo.setData((float*)&lines[0], sizeof(LinePoints) * NUM_LINES, GL_DYNAMIC_DRAW);
}

void NormalRenderer::setEndPoint(int idx, glm::vec3 endPoint) {
	// float* vertices = lines[idx].end;
	// vertices[0] = endPoint.x;
	// vertices[1] = endPoint.y;
	// vertices[2] = endPoint.z;
	// vertices[0] = 0;
	// vertices[1] = 200;
	// vertices[2] = 0;
	if (idx >= NUM_LINES) return;
	lines[idx].end[0] = endPoint.x;
	lines[idx].end[1] = endPoint.y;
	lines[idx].end[2] = endPoint.z;
	vbo.setData((float*)&lines[0], sizeof(LinePoints) * NUM_LINES, GL_DYNAMIC_DRAW);
}

