#pragma once

#include "fbxLoader.h"
#include "shaderProgram.h"
#include "vao.h"
#include "vbo.h"

class MeshRenderer {
public:
	MeshRenderer() {}
	MeshRenderer(Mesh mesh, ShaderProgram shaderProgram);
	void render();

	bool displaySplitNormals = false;
	bool displayNormals = false;

	Mesh mesh;

private:
	ShaderProgram shaderProgram;
	VAO vao;
	VBO vbo;
};
