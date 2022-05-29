#include "line.h"

Line::Line() {
	vertices[0] = 0.0f;
	vertices[1] = 0.0f;
	vertices[2] = 0.0f;
	vertices[3] = 0.0f;
	vertices[4] = 1000.0f;
	vertices[5] = 0.0f;

	const char* lineVert = "C:\\Sarthak\\programming\\3dFileLoader\\Editor\\src\\shaders\\line.vert";
	const char* lineFrag = "C:\\Sarthak\\programming\\3dFileLoader\\Editor\\src\\shaders\\line.frag";

	shaderProgram = ShaderProgram(lineVert, lineFrag);

	vao.bind();
	vbo.setData(vertices, sizeof(vertices), GL_STATIC_DRAW);
	// vao.attachVBO(vbo, 0, 3, 3, 0);
	vao.attachVBO(vbo, 0, 3, 3 * sizeof(float), 0);
	vao.unbind();

	setColor(glm::vec3(1.0f, 0.0f, 0.0f));
}

void Line::setColor(glm::vec3 color) {
	shaderProgram.setVec3("color", color);
}

void Line::setStartPoint(glm::vec3 startPoint) {
	vertices[0] = startPoint.x;
	vertices[1] = startPoint.y;
	vertices[2] = startPoint.z;
	vbo.setData(vertices, sizeof(vertices), GL_STATIC_DRAW);
}

void Line::setEndPoint(glm::vec3 endPoint) {
	vertices[3] = endPoint.x;
	vertices[4] = endPoint.y;
	vertices[5] = endPoint.z;
	vbo.setData(vertices, sizeof(vertices), GL_STATIC_DRAW);
}

void Line::render() {
	shaderProgram.bind();
	vao.bind();
	glDrawArrays(GL_LINES, 0, 2);
	vao.unbind();
	shaderProgram.unbind();
}
