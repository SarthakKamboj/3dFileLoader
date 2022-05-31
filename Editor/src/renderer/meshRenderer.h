#pragma once

#include "fbxLoader.h"
#include "shaderProgram.h"
#include "vao.h"
#include "vbo.h"

class MeshRenderer {
public:
	MeshRenderer(Mesh mesh, ShaderProgram shaderProgram);
	void render();

	bool displayNormals = false;

private:
	Mesh mesh;
	ShaderProgram shaderProgram;
	VAO vao;
	VBO vbo;
};
