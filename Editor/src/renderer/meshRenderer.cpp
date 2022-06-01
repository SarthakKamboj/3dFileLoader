#include "meshRenderer.h"
#include "line.h"
#include "glm/glm.hpp"
#include "glm/gtc/type_ptr.hpp"

extern glm::mat4 getRotationMatrix(glm::vec3 rot);
extern Line* linePtr;

MeshRenderer::MeshRenderer() {
	mesh = NULL;
	const char* defaultVert = "C:\\Sarthak\\programming\\3dFileLoader\\Editor\\src\\shaders\\default.vert";
	const char* defaultFrag = "C:\\Sarthak\\programming\\3dFileLoader\\Editor\\src\\shaders\\default.frag";
	shaderProgram = ShaderProgram(defaultVert, defaultFrag);
	shaderProgram.setVec3("color", glm::vec3(1, 1, 1));
	shaderProgram.setFloat("displacement", 0);
}

MeshRenderer::MeshRenderer(Mesh* _mesh) {
	mesh = _mesh;

	const char* defaultVert = "C:\\Sarthak\\programming\\3dFileLoader\\Editor\\src\\shaders\\default.vert";
	const char* defaultFrag = "C:\\Sarthak\\programming\\3dFileLoader\\Editor\\src\\shaders\\default.frag";
	shaderProgram = ShaderProgram(defaultVert, defaultFrag);
	shaderProgram.setVec3("color", glm::vec3(1, 1, 1));
	shaderProgram.setFloat("displacement", 0);

	vao.bind();

	vbo.setData((float*)&mesh->vertices[0], mesh->vertexCount * sizeof(Vertex), GL_STATIC_DRAW);

	vao.attachVBO(vbo, 0, 3, sizeof(Vertex), offsetof(Vertex, position));
	vao.attachVBO(vbo, 1, 3, sizeof(Vertex), offsetof(Vertex, color));
	vao.attachVBO(vbo, 2, 2, sizeof(Vertex), offsetof(Vertex, uvs));
	vao.attachVBO(vbo, 3, 3, sizeof(Vertex), offsetof(Vertex, avgNormal));

	vao.unbind();
	vbo.unbind();


}

void MeshRenderer::render() {

	glm::mat4 translation = glm::translate(glm::mat4(1.0f), mesh->transform.position);
	glm::mat4 rotation = getRotationMatrix(mesh->transform.rotation);
	glm::mat4 scale = glm::scale(glm::mat4(1.0f), mesh->transform.scale);

	glm::mat4 model = translation * rotation * scale;

	if (displayNormals | displaySplitNormals) {
		linePtr->shaderProgram.setMat4("model", model);
		for (int vertexIdx = 0; vertexIdx < mesh->vertexCount; vertexIdx++) {
			Vertex& vert = mesh->vertices[vertexIdx];
			glm::vec3 startPoint(vert.position[0], vert.position[1], vert.position[2]);
			glm::vec3 endPoint = startPoint + 100.0f * glm::vec3(vert.normal[0], vert.normal[1], vert.normal[2]);
			glm::vec3 avgEndPoint = startPoint + 1.25f * 100.0f * glm::vec3(vert.avgNormal[0], vert.avgNormal[1], vert.avgNormal[2]);
			linePtr->setStartPoint(startPoint);

			if (displaySplitNormals) {
				linePtr->setColor(glm::vec3(1, 0, 0));
				linePtr->setEndPoint(endPoint);
				linePtr->render();
			}

			if (displayNormals) {
				linePtr->setColor(glm::vec3(0, 0, 1));
				linePtr->setEndPoint(avgEndPoint);
				linePtr->render();
			}

		}
	}

	shaderProgram.setMat4("model", model);
	shaderProgram.bind();
	if (wireframeMode) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}
	if (shaderProgram.textureBasedColor) {
		shaderProgram.texture.bind();
	}
	vao.bind();
	glDrawArrays(GL_TRIANGLES, 0, mesh->vertexCount);
	vao.unbind();
	if (shaderProgram.textureBasedColor) {
		shaderProgram.texture.unbind();
	}
	if (wireframeMode) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}
	shaderProgram.unbind();
}