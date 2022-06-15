#include "meshRenderer.h"
#include "line.h"
#include "glm/glm.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "panels/shaderRegistry.h"
#include "fbxLoader.h"
#include "panels/sceneList.h"
#include "panels/panelsManager.h"
#include "renderer/normalRenderer.h"
#include <iostream>

extern glm::mat4 getRotationMatrix(glm::vec3 rot);
extern NormalRenderer* normalRendererPtr;
extern NormalRenderer* splitNormalRendererPtr;
extern PanelsManager* g_PanelsManager;

MeshRenderer::MeshRenderer() {
	meshIdx = -1;
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
}

void MeshRenderer::render() {

	SceneList* sceneListPtr = &g_PanelsManager->sceneList;
	if (sceneListPtr->curSceneIdx < 0) return;

	Scene* scenePtr = &sceneListPtr->scenes[sceneListPtr->curSceneIdx];
	Mesh* mesh = &scenePtr->meshes[meshIdx];
	glm::mat4 translation = glm::translate(glm::mat4(1.0f), mesh->transform.position);
	glm::mat4 rotation = getRotationMatrix(mesh->transform.rotation);
	glm::mat4 scale = glm::scale(glm::mat4(1.0f), mesh->transform.scale);

	glm::mat4 model = translation * rotation * scale;

	if (displayNormals | displaySplitNormals) {

		for (int vertexIdx = 0; vertexIdx < mesh->vertexCount; vertexIdx++) {
			Vertex& vert = mesh->vertices[vertexIdx];
			glm::vec4 startPoint(vert.position[0], vert.position[1], vert.position[2], 1);
			glm::vec4 endPoint = startPoint + 100.0f * glm::vec4(vert.normal[0], vert.normal[1], vert.normal[2], 0.0f);
			glm::vec4 avgEndPoint = startPoint + 1.25f * 100.0f * glm::vec4(vert.avgNormal[0], vert.avgNormal[1], vert.avgNormal[2], 0.0f);

			if (displaySplitNormals) {
				splitNormalRendererPtr->setStartPoint(vertexIdx, startPoint);
				splitNormalRendererPtr->setEndPoint(vertexIdx, endPoint);
			}

			if (displayNormals) {
				normalRendererPtr->setStartPoint(vertexIdx, startPoint);
				normalRendererPtr->setEndPoint(vertexIdx, avgEndPoint);
			}

		}

		if (displaySplitNormals) {
			splitNormalRendererPtr->shaderProgram.setMat4("model", model);
			splitNormalRendererPtr->render(mesh->vertexCount);
		}

		if (displayNormals) {
			normalRendererPtr->shaderProgram.setMat4("model", model);
			normalRendererPtr->render(mesh->vertexCount);
		}

	}

	ShaderRegistry* shaderRegistryPtr = &g_PanelsManager->shaderRegistry;
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