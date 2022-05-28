#pragma once

#include "glm/glm.hpp"

struct Vertex {
	float position[3];
	float color[3];
	float uvs[2];
	float normal[3];
};

struct Mesh {

	Vertex* vertices;
	int vertexCount;
	unsigned int* indicies;
	int indexCount;

	glm::vec3 position;
	glm::vec3 rotation;
	glm::vec3 scale;
};

struct SceneData {
	Mesh* meshes;
	int meshCount;
};

SceneData loadFbx(const char* fbxFilePath);
