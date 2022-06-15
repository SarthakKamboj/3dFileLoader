#pragma once

#define MAX_NUM_MESHES_PER_SCENE 10

#include "glm/glm.hpp"
#include <string>

struct Vertex {
	float position[3];
	float color[3];
	float uvs[2];
	float normal[3];
	float avgNormal[3];
};

struct Transform {
	Transform() {
		position = glm::vec3(0, 0, 0);
		rotation = glm::vec3(0, 0, 0);
		scale = glm::vec3(1, 1, 1);
	}

	glm::vec3 position;
	glm::vec3 rotation;
	glm::vec3 scale;
};

struct Mesh {
	Mesh() {
		vertices = NULL;
		vertexCount = -1;
		numChildren = 0;
		name = "";
	}

	Vertex* vertices;
	int vertexCount;

	Transform transform;

	int numChildren = 0;
	int childMeshIdxs[MAX_NUM_MESHES_PER_SCENE];

	std::string name;
};


struct Scene {
	Mesh meshes[MAX_NUM_MESHES_PER_SCENE] = {};
	int numMeshes = 0;
	int topLevelMeshIdxs[MAX_NUM_MESHES_PER_SCENE] = {};
	char name[150];
};

Scene loadFbx(const char* fbxFilePath);
