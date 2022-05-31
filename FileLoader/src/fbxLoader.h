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

	/* not having indicies for now b / c mapping over vertices by polygon,
	which means number of vertices will be the same as indicies
	*/
	// unsigned int* indicies;
	// int indexCount;

	Transform transform;

	int numChildren;
	std::vector<int> childMeshIdxs;

	std::string name;
};

struct Camera {
	Camera() {}

	Transform transform;
};

struct Light {
	Light();
	Transform transform;
};

struct Scene {
	std::vector<Mesh> meshes;
	int numMeshes;
	std::vector<int> topLevelMeshIdxs;
};

Scene loadFbx(const char* fbxFilePath);
