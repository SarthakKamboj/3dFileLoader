#include "meshRenderer.h"
#include "glm/glm.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "panels/shaderRegistry.h"
#include "fbxLoader.h"
#include "panels/sceneList.h"
#include "panels/panelsManager.h"
#include "renderer/normalRenderer.h"
#include <iostream>
#include "helper.h"

extern NormalRenderer* g_NormalRendererPtr;
extern NormalRenderer* g_SplitNormalRendererPtr;
extern PanelsManager* g_PanelsManager;

MeshRenderer::MeshRenderer() {
	meshIdx = -1;
	createWireframeShader();
}

MeshRenderer::MeshRenderer(const Mesh& _mesh, int _meshIdx) {
	createWireframeShader();

	meshIdx = _meshIdx;
	const Mesh* mesh = &_mesh;

	vao.bind();

	// set mesh vertex data in vbo
	vbo.setData((float*)&mesh->vertices[0], mesh->vertexCount * sizeof(Vertex), GL_STATIC_DRAW);

	vao.attachVBO(vbo, 0, 3, sizeof(Vertex), offsetof(Vertex, position));
	vao.attachVBO(vbo, 1, 3, sizeof(Vertex), offsetof(Vertex, color));
	vao.attachVBO(vbo, 2, 2, sizeof(Vertex), offsetof(Vertex, uvs));
	vao.attachVBO(vbo, 3, 3, sizeof(Vertex), offsetof(Vertex, avgNormal));
	vao.attachVBO(vbo, 4, 3, sizeof(Vertex), offsetof(Vertex, normal));

	vao.unbind();
	vbo.unbind();
}

void MeshRenderer::createWireframeShader() {
	// get wireframe shader paths
	char wireframeVert[200] = {};
	char wireframeFrag[200] = {};
	Helper::GetPathForPredefinedShader(wireframeVert, "wireframe.vert");
	Helper::GetPathForPredefinedShader(wireframeFrag, "wireframe.frag");

	wireFrameShaderProgram = ShaderProgram(wireframeVert, wireframeFrag);
}

void MeshRenderer::render() {

	SceneList* sceneListPtr = &g_PanelsManager->sceneList;
	if (sceneListPtr->curSceneIdx < 0) return;

	// get mesh model matrix
	Scene* scenePtr = &sceneListPtr->scenes[sceneListPtr->curSceneIdx];
	Mesh* mesh = &scenePtr->meshes[meshIdx];
	glm::mat4 translation = glm::translate(glm::mat4(1.0f), mesh->transform.position);
	glm::mat4 rotation = Helper::GetRotationMatrix(mesh->transform.rotation);
	glm::mat4 scale = glm::scale(glm::mat4(1.0f), mesh->transform.scale);

	glm::mat4 model = translation * rotation * scale;
	ShaderRegistry* shaderRegistryPtr = &g_PanelsManager->shaderRegistry;
	ShaderProgram& curShader = shaderRegistryPtr->shaders[shaderIdx];

	if (wireframeMode) {
		GLint splitLoc = glGetUniformLocation(curShader.programId, "useSplit");
		GLint displacementLoc = glGetUniformLocation(curShader.programId, "displacement");

		GLfloat displacement;
		glGetUniformfv(curShader.programId, displacementLoc, &displacement);

		wireFrameShaderProgram.setMat4("model", model);
		wireFrameShaderProgram.setFloat("displacement", displacement);
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		wireFrameShaderProgram.bind();
		vao.bind();
		glDrawArrays(GL_TRIANGLES, 0, mesh->vertexCount);
		vao.unbind();
		wireFrameShaderProgram.unbind();
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		return;
	}

	if (displayNormals | displaySplitNormals) {

		// if should display normals, calculate those normals and update the normal renderers accordingly
		for (int vertexIdx = 0; vertexIdx < mesh->vertexCount; vertexIdx++) {
			Vertex& vert = mesh->vertices[vertexIdx];
			glm::vec4 startPoint(vert.position[0], vert.position[1], vert.position[2], 1);
			glm::vec4 endPoint = startPoint + 100.0f * glm::vec4(vert.normal[0], vert.normal[1], vert.normal[2], 0.0f);
			glm::vec4 avgEndPoint = startPoint + 1.25f * 100.0f * glm::vec4(vert.avgNormal[0], vert.avgNormal[1], vert.avgNormal[2], 0.0f);

			if (displaySplitNormals) {
				g_SplitNormalRendererPtr->setStartPoint(vertexIdx, startPoint);
				g_SplitNormalRendererPtr->setEndPoint(vertexIdx, endPoint);
			}

			if (displayNormals) {
				g_NormalRendererPtr->setStartPoint(vertexIdx, startPoint);
				g_NormalRendererPtr->setEndPoint(vertexIdx, avgEndPoint);
			}

		}

		if (displaySplitNormals) {
			g_SplitNormalRendererPtr->shaderProgram.setMat4("model", model);
			g_SplitNormalRendererPtr->render(mesh->vertexCount);
		}

		if (displayNormals) {
			g_NormalRendererPtr->shaderProgram.setMat4("model", model);
			g_NormalRendererPtr->render(mesh->vertexCount);
		}

	}

	// set shader data and render mesh
	shaderRegistryPtr->shaders[shaderIdx].setMat4("model", model);
	shaderRegistryPtr->shaders[shaderIdx].setInt("useSplit", !useNormals);
	shaderRegistryPtr->shaders[shaderIdx].bind();
	// if (wireframeMode) {
		// glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	// }
	if (shaderRegistryPtr->shaders[shaderIdx].textureBasedColor) {
		shaderRegistryPtr->shaders[shaderIdx].texture.bind();
	}
	vao.bind();
	glDrawArrays(GL_TRIANGLES, 0, mesh->vertexCount);
	vao.unbind();
	if (shaderRegistryPtr->shaders[shaderIdx].textureBasedColor) {
		shaderRegistryPtr->shaders[shaderIdx].texture.unbind();
	}
	// if (wireframeMode) {
		// glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	// }
	shaderRegistryPtr->shaders[shaderIdx].unbind();

}