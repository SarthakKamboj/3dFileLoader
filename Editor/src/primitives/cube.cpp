#include "cube.h"
#include "helper.h"

Cube::Cube() {
	vbo.setData(vertices, sizeof(vertices), GL_STATIC_DRAW);
	vao.bind();
	vao.attachVBO(vbo, 0, 3, 6 * sizeof(float), 0);
	vao.unbind();

	char vert[200] = {};
	char frag[200] = {};
	Helper::GetApplicationPath(vert);
	Helper::GetApplicationPath(frag);
	const char* relativeShadersFolderPath = "\\src\\shaders\\";
	Helper::ConcatBuffer(vert, relativeShadersFolderPath);
	Helper::ConcatBuffer(frag, relativeShadersFolderPath);
	Helper::ConcatBuffer(vert, "cube.vert");
	Helper::ConcatBuffer(frag, "cube.frag");

	shaderProgram = ShaderProgram(vert, frag);

	shaderProgram.setVec3("color", glm::vec3(1, 1, 1));
}

void Cube::render() {
	shaderProgram.bind();
	vao.bind();
	glDrawArrays(GL_TRIANGLES, 0, 36);
	vao.unbind();
	shaderProgram.unbind();
}
