#include "normalRenderer.h"

NormalRenderer::NormalRenderer() {
	// set vbo to house NUM_LINES Linepoints
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

void NormalRenderer::setColor(const glm::vec3& color) {
	shaderProgram.setVec3("color", color);
}

// render specific number of lines and not unncessary render too many for some meshes
void NormalRenderer::render(int numLines) {
	shaderProgram.bind();
	vao.bind();
	glDrawArrays(GL_LINES, 0, numLines * 2);
	vao.unbind();
	shaderProgram.unbind();
}

void NormalRenderer::setStartPoint(int idx, const glm::vec3& startPoint) {
	if (idx >= NUM_LINES) return;

	lines[idx].start[0] = startPoint.x;
	lines[idx].start[1] = startPoint.y;
	lines[idx].start[2] = startPoint.z;

	vbo.setData((float*)&lines[0], sizeof(LinePoints) * NUM_LINES, GL_DYNAMIC_DRAW);
}

void NormalRenderer::setEndPoint(int idx, const glm::vec3& endPoint) {
	if (idx >= NUM_LINES) return;
	lines[idx].end[0] = endPoint.x;
	lines[idx].end[1] = endPoint.y;
	lines[idx].end[2] = endPoint.z;
	vbo.setData((float*)&lines[0], sizeof(LinePoints) * NUM_LINES, GL_DYNAMIC_DRAW);
}

