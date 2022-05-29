#pragma once

#include "glm/glm.hpp"
#include <string>
#include <vector>

struct Vertex {
	float position[3];
	float color[3];
	float uvs[2];
	float normal[3];
	float avgNormal[3];
	int positionId;
};

struct Mesh {
	Vertex* vertices;
	int vertexCount;
	unsigned int* indicies;
	int indexCount;

	glm::vec3 position;
	glm::vec3 rotation;
	glm::vec3 scale;

	int numChildren;
	std::vector<Mesh> childMeshes;

	std::string name;
};

struct Scene {
	std::vector<Mesh> meshes;
	int numMeshes;
};

Scene loadFbx(const char* fbxFilePath);
