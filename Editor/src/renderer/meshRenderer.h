#pragma once

#include "fbxLoader.h"
#include "shaderProgram.h"
#include "vao.h"
#include "vbo.h"

class MeshRenderer {
public:
	MeshRenderer();
	MeshRenderer(Mesh* mesh);
	void render();

	bool displaySplitNormals = false;
	bool displayNormals = false;
	bool wireframeMode = false;

	Mesh* mesh;
	ShaderProgram shaderProgram;

private:
	VAO vao;
	VBO vbo;
};
