#include "sceneRenderer.h"
#include "renderer/normalRenderer.h"
#include "imgui.h"

extern glm::mat4 getRotationMatrix(glm::vec3 rot);
extern PanelsManager* g_PanelsManager;
extern NormalRenderer* splitNormalRendererPtr;
extern NormalRenderer* normalRendererPtr;
extern int width, height;


SceneRenderer::SceneRenderer(Light* light) {
	const char* lightPassVert = "C:\\Sarthak\\programming\\3dFileLoader\\Editor\\src\\shaders\\lightPass.vert";
	const char* lightPassFrag = "C:\\Sarthak\\programming\\3dFileLoader\\Editor\\src\\shaders\\lightPass.frag";
	lightPassShader = ShaderProgram(lightPassVert, lightPassFrag);
	lightFrameBuffer.light = light;
}

void SceneRenderer::renderScene() {
	CameraPanel& cameraPanel = g_PanelsManager->cameraPanel;
	SceneList& sceneList = g_PanelsManager->sceneList;

	/*
		LIGHT PASS
	*/
	lightFrameBuffer.bind();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);
	glViewport(0, 0, width, height);
	glm::mat4 lightView = lightFrameBuffer.getLightViewMat();
	glm::mat4 lightProj = cameraPanel.getProjectionMat();
	glm::mat4 cameraProj = cameraPanel.getProjectionMat();

	lightPassShader.setMat4("lightView", lightView);
	lightPassShader.setMat4("lightProj", lightProj);
	lightPassShader.bind();
	if (sceneList.curSceneIdx > -1) {
		Scene& scene = sceneList.scenes[sceneList.curSceneIdx];
		MeshRenderer* meshRenderers = sceneList.meshRenderLists[sceneList.curSceneIdx];
		glm::vec3 camPos(cameraPanel.radius * cos(glm::radians(cameraPanel.angle)), cameraPanel.yPos, cameraPanel.radius * sin(glm::radians(cameraPanel.angle)));
		for (int meshId = 0; meshId < scene.numMeshes; meshId++) {
			Mesh* mesh = &scene.meshes[meshRenderers[meshId].meshIdx];
			glm::mat4 translation = glm::translate(glm::mat4(1.0f), mesh->transform.position);
			glm::mat4 rotation = getRotationMatrix(mesh->transform.rotation);
			glm::mat4 scale = glm::scale(glm::mat4(1.0f), mesh->transform.scale);

			glm::mat4 model = translation * rotation * scale;
			lightPassShader.setMat4("model", model);

			lightPassShader.bind();
			meshRenderers[meshId].vao.bind();
			glDrawArrays(GL_TRIANGLES, 0, mesh->vertexCount);
			meshRenderers[meshId].vao.unbind();
		}
	}

	lightPassShader.unbind();
	lightFrameBuffer.unbind();
	glBindTexture(GL_TEXTURE_2D, 0);

	/*
		SCENE PASS
	*/
	glm::mat4 view = cameraPanel.getViewMat();
	normalRendererPtr->shaderProgram.setMat4("view", view);
	normalRendererPtr->shaderProgram.setMat4("projection", cameraProj);
	splitNormalRendererPtr->shaderProgram.setMat4("view", view);
	splitNormalRendererPtr->shaderProgram.setMat4("projection", cameraProj);

	glViewport(0, 0, width, height);

	sceneFbo.bind();
	FrameBuffer::ClearBuffers(glm::vec3(0, 0, 0));
	glEnable(GL_DEPTH_TEST);
	glViewport(0, 0, width, height);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, lightFrameBuffer.depthTexture);

	Light* light = lightFrameBuffer.light;
	if (sceneList.curSceneIdx > -1) {
		Scene& scene = sceneList.scenes[sceneList.curSceneIdx];
		MeshRenderer* meshRenderers = sceneList.meshRenderLists[sceneList.curSceneIdx];
		glm::vec3 camPos(cameraPanel.radius * cos(glm::radians(cameraPanel.angle)), cameraPanel.yPos, cameraPanel.radius * sin(glm::radians(cameraPanel.angle)));
		ShaderRegistry& shaderRegistry = g_PanelsManager->shaderRegistry;
		for (int meshId = 0; meshId < scene.numMeshes; meshId++) {

			int shaderIdx = meshRenderers[meshId].shaderIdx;
			shaderRegistry.shaders[shaderIdx].setInt("depthTexUnit", 1);
			shaderRegistry.shaders[shaderIdx].setMat4("lightProj", lightProj);
			shaderRegistry.shaders[shaderIdx].setMat4("lightView", lightView);

			shaderRegistry.shaders[shaderIdx].setMat4("view", view);
			shaderRegistry.shaders[shaderIdx].setMat4("projection", cameraProj);

			shaderRegistry.shaders[shaderIdx].setFloat("light.ambientFactor", light->ambientFactor);
			shaderRegistry.shaders[shaderIdx].setVec3("light.color", light->lightColor);
			shaderRegistry.shaders[shaderIdx].setVec3("light.pos", light->pos);

			shaderRegistry.shaders[shaderIdx].setFloat("material.specularStrength", light->specularFactor);
			shaderRegistry.shaders[shaderIdx].setFloat("material.shininess", light->shininess);

			shaderRegistry.shaders[shaderIdx].setVec3("viewPos", camPos);

			meshRenderers[meshId].render();
		}

		lightCube.shaderProgram.setVec3("color", light->lightColor);
		glm::mat4 translation = glm::translate(glm::mat4(1.0f), light->pos);
		glm::mat4 rotation(1.0f);
		glm::mat4 scale = glm::scale(glm::mat4(1.0f), glm::vec3(100.0f, 100.0f, 100.0f));

		glm::mat4 lightModel = translation * rotation * scale;
		lightCube.shaderProgram.setVec3("color", light->lightColor);
		lightCube.shaderProgram.setMat4("model", lightModel);
		lightCube.shaderProgram.setMat4("view", view);
		lightCube.shaderProgram.setMat4("projection", cameraProj);
		lightCube.render();
	}

	sceneFbo.unbind();
}