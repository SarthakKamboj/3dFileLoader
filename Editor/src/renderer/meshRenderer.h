#pragma once

#include "fbxLoader.h"
#include "shaderProgram.h"
#include "vao.h"
#include "vbo.h"
#include "texture.h"

struct MeshRenderer {
	MeshRenderer();
	MeshRenderer(Mesh* mesh);
	void render();

	bool displaySplitNormals = false;
	bool displayNormals = false;
	bool wireframeMode = false;

	Mesh* mesh;
	// ShaderProgram* shaderProgram;
	int shaderIdx = 0;

	VAO vao;
	VBO vbo;
};
