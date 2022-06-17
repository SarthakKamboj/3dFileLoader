#pragma once

#include "fbxLoader.h"
#include "shaderProgram.h"
#include "vao.h"
#include "vbo.h"
#include "texture.h"

struct MeshRenderer {
	MeshRenderer();
	MeshRenderer(const Mesh& mesh, int _meshIdx);
	void render();

	bool displaySplitNormals = false;
	bool displayNormals = false;
	bool wireframeMode = false;
	bool useNormals = false;

	int meshIdx;
	int shaderIdx = 0;

	ShaderProgram wireFrameShaderProgram;

	VAO vao;
	VBO vbo;

private:
	void createWireframeShader();
};
