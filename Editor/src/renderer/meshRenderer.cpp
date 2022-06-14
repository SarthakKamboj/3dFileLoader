#include "meshRenderer.h"
#include "line.h"
#include "glm/glm.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "panels/shaderRegistry.h"
#include "fbxLoader.h"
#include "panels/sceneList.h"
#include "renderer/normalRenderer.h"
#include <iostream>

extern glm::mat4 getRotationMatrix(glm::vec3 rot);
// extern Line* linePtr;
extern NormalRenderer* normalRendererPtr;
extern NormalRenderer* splitNormalRendererPtr;
extern ShaderRegistry* shaderRegistryPtr;
extern SceneList* sceneListPtr;

MeshRenderer::MeshRenderer() {
	meshIdx = -1;
	// normalIdx = -1;
	// splitNormalIdx = -1;
}

MeshRenderer::MeshRenderer(Mesh& _mesh, int _meshIdx) {
	meshIdx = _meshIdx;
	Mesh* mesh = &_mesh;

	vao.bind();

	vbo.setData((float*)&mesh->vertices[0], mesh->vertexCount * sizeof(Vertex), GL_STATIC_DRAW);

	vao.attachVBO(vbo, 0, 3, sizeof(Vertex), offsetof(Vertex, position));
	vao.attachVBO(vbo, 1, 3, sizeof(Vertex), offsetof(Vertex, color));
	vao.attachVBO(vbo, 2, 2, sizeof(Vertex), offsetof(Vertex, uvs));
	vao.attachVBO(vbo, 3, 3, sizeof(Vertex), offsetof(Vertex, avgNormal));
	vao.attachVBO(vbo, 4, 3, sizeof(Vertex), offsetof(Vertex, normal));

	vao.unbind();
	vbo.unbind();
	// normalIdx = normalRendererPtr->receiveIdx();
	// splitNormalIdx = splitNormalRendererPtr->receiveIdx();
}

void MeshRenderer::render() {

	if (sceneListPtr->curSceneIdx < 0) return;

	Scene* scenePtr = &sceneListPtr->scenes[sceneListPtr->curSceneIdx];
	Mesh* mesh = &scenePtr->meshes[meshIdx];
	glm::mat4 translation = glm::translate(glm::mat4(1.0f), mesh->transform.position);
	glm::mat4 rotation = getRotationMatrix(mesh->transform.rotation);
	glm::mat4 scale = glm::scale(glm::mat4(1.0f), mesh->transform.scale);

	glm::mat4 model = translation * rotation * scale;

	// memset(normalRendererPtr->lines, 0, NUM_LINES * sizeof(LinePoints));
	// memset(splitNormalRendererPtr->lines, 0, NUM_LINES * sizeof(LinePoints));
	if (displayNormals | displaySplitNormals) {
		// linePtr->shaderProgram.setMat4("model", model);
		// normalRendererPtr->shaderProgram.setMat4("model", model);

		for (int vertexIdx = 0; vertexIdx < mesh->vertexCount; vertexIdx++) {
			Vertex& vert = mesh->vertices[vertexIdx];
			glm::vec4 startPoint(vert.position[0], vert.position[1], vert.position[2], 1);
			glm::vec4 endPoint = startPoint + 100.0f * glm::vec4(vert.normal[0], vert.normal[1], vert.normal[2], 0.0f);
			glm::vec4 avgEndPoint = startPoint + 1.25f * 100.0f * glm::vec4(vert.avgNormal[0], vert.avgNormal[1], vert.avgNormal[2], 0.0f);

			// glm::vec3 startPoint = model * lclStartPoint;
			// glm::vec3 endPoint = model * lclEndPoint;
			// glm::vec3 avgEndPoint = model * lclAvgEndPoint;

			// startPoint = glm::vec3(0, 0, 0);
			// endPoint = glm::vec3(0, 200, 0);
			// avgEndPoint = glm::vec3(100, 0, 0);

			if (displaySplitNormals) {
				/*
				splitNormalRendererPtr->lines[normalIdx].start[0] = startPoint[0];
				splitNormalRendererPtr->lines[normalIdx].start[1] = startPoint[1];
				splitNormalRendererPtr->lines[normalIdx].start[2] = startPoint[2];

				splitNormalRendererPtr->lines[normalIdx].end[0] = endPoint[0];
				splitNormalRendererPtr->lines[normalIdx].end[1] = endPoint[1];
				splitNormalRendererPtr->lines[normalIdx].end[2] = endPoint[2];
				*/
				// splitNormalRendererPtr->setStartPoint(splitNormalIdx, startPoint);
				// splitNormalRendererPtr->setEndPoint(splitNormalIdx, endPoint);


				splitNormalRendererPtr->setStartPoint(vertexIdx, startPoint);
				splitNormalRendererPtr->setEndPoint(vertexIdx, endPoint);
			}

			if (displayNormals) {
				/*
				normalRendererPtr->lines[normalIdx].start[0] = startPoint[0];
				normalRendererPtr->lines[normalIdx].start[1] = startPoint[1];
				normalRendererPtr->lines[normalIdx].start[2] = startPoint[2];

				normalRendererPtr->lines[normalIdx].end[0] = avgEndPoint[0];
				normalRendererPtr->lines[normalIdx].end[1] = avgEndPoint[1];
				normalRendererPtr->lines[normalIdx].end[2] = avgEndPoint[2];
				*/
				// normalRendererPtr->setStartPoint(normalIdx, startPoint);
				// normalRendererPtr->setEndPoint(normalIdx, avgEndPoint);


				normalRendererPtr->setStartPoint(vertexIdx, startPoint);
				normalRendererPtr->setEndPoint(vertexIdx, avgEndPoint);
			}

		}

		if (displaySplitNormals) {
			splitNormalRendererPtr->shaderProgram.setMat4("model", model);
			splitNormalRendererPtr->render(mesh->vertexCount);
			std::cout << mesh->vertexCount << std::endl;
		}

		if (displayNormals) {
			normalRendererPtr->shaderProgram.setMat4("model", model);
			normalRendererPtr->render(mesh->vertexCount);
			std::cout << mesh->vertexCount << std::endl;
		}



	}

	shaderRegistryPtr->shaders[shaderIdx].setMat4("model", model);
	shaderRegistryPtr->shaders[shaderIdx].setInt("useSplit", !useNormals);
	shaderRegistryPtr->shaders[shaderIdx].bind();
	if (wireframeMode) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}
	if (shaderRegistryPtr->shaders[shaderIdx].textureBasedColor) {
		shaderRegistryPtr->shaders[shaderIdx].texture.bind();
	}
	vao.bind();
	glDrawArrays(GL_TRIANGLES, 0, mesh->vertexCount);
	vao.unbind();
	if (shaderRegistryPtr->shaders[shaderIdx].textureBasedColor) {
		shaderRegistryPtr->shaders[shaderIdx].texture.unbind();
	}
	if (wireframeMode) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}
	shaderRegistryPtr->shaders[shaderIdx].unbind();

}