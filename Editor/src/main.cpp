#include <iostream>
#include "SDL.h"
#include "glad/glad.h"
#include "stb_image.h"
#include "glm/glm.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "fbxLoader.h"
#include <fstream>
#include <string>
#include "renderer/shaderProgram.h"
#include "renderer/vao.h"
#include "renderer/vbo.h"
#include "renderer/ebo.h"
#include "renderer/meshRenderer.h"
#include "renderer/frameBuffer.h"
#include "renderer/texture.h"
#include "renderer/line.h"
#include "panels/meshRendererSettingsPanel.h"
#include "panels/sceneHierarchyPanel.h"
#include <vector>
#include "imgui.h"
#include "imgui_impl_opengl3.h"
#include "imgui_impl_sdl.h"
#include "stb_image.h"
#include "panels/dockspace.h"
#include "panels/cameraPanel.h"
#include "panels/shaderEditor.h"
#include "panels/fileBrowser.h"
#include "panels/shaderRegistry.h"
#include "helper.h"
#include "panels/sceneList.h"
#include "renderer/light.h"
#include "primitives/cube.h"
#include "renderer/lightFrameBuffer.h"
#include "glm/gtc/type_ptr.hpp"
#include "renderer/normalRenderer.h"
#include "window.h"
#include "input.h"
#include "panels/panelsManager.h"

int width = 600, height = 600;
NormalRenderer* splitNormalRendererPtr;
NormalRenderer* normalRendererPtr;
PanelsManager* g_PanelsManager;
Input* g_Input;
// bool enterPressed = false;

extern float nearPlane, farPlane;

void setSceneViewWindowConstraint(ImGuiSizeCallbackData* data) {
	data->DesiredSize.y = data->DesiredSize.x * (((float)height) / width);
}

glm::mat4 getRotationMatrix(glm::vec3 rot) {
	glm::mat4 rotMatrix(1.0f);
	rotMatrix = glm::rotate(rotMatrix, glm::radians(rot.x), glm::vec3(1, 0, 0));
	rotMatrix = glm::rotate(rotMatrix, glm::radians(rot.y), glm::vec3(0, 1, 0));
	rotMatrix = glm::rotate(rotMatrix, glm::radians(rot.z), glm::vec3(0, 0, 1));
	return rotMatrix;
}

float quadVertices[] = {
	// positions   // texCoords
	-1.0f,  1.0f,  0.0f, 1.0f,
	-1.0f, -1.0f,  0.0f, 0.0f,
	 1.0f, -1.0f,  1.0f, 0.0f,

	-1.0f,  1.0f,  0.0f, 1.0f,
	 1.0f, -1.0f,  1.0f, 0.0f,
	 1.0f,  1.0f,  1.0f, 1.0f
};

int main(int argc, char* args[]) {

	Input input;
	g_Input = &input;
	Window window(g_Input);
	PanelsManager panelsManager;
	g_PanelsManager = &panelsManager;

	/*
	ShaderRegistry shaderRegistry;
	shaderRegistryPtr = &shaderRegistry;
	*/

	NormalRenderer splitNormalRenderer;
	splitNormalRendererPtr = &splitNormalRenderer;
	splitNormalRendererPtr->setColor(glm::vec3(1, 0, 0));

	NormalRenderer normalRenderer;
	normalRendererPtr = &normalRenderer;

	glEnable(GL_DEPTH_TEST);

	FrameBuffer sceneFbo;


	char fbxToLoadPath[300] = {};
	bool selectingFbxToLoad = false;

	Cube lightCube;

	Light light = {};
	light.ambientColor = glm::vec3(1, 0, 0);
	light.ambientFactor = 0.2f;
	light.specularFactor = 0.2f;
	light.pos = glm::vec3(10, 700, 900);
	light.lightColor = glm::vec3(0, 1, 0);
	light.shininess = 32.0f;

	LightFrameBuffer lightFrameBuffer;
	lightFrameBuffer.light = &light;

	VAO depthVAO;
	VBO depthVBO;
	depthVBO.bind();
	depthVBO.setData(quadVertices, sizeof(quadVertices), GL_STATIC_DRAW);
	depthVAO.bind();
	depthVAO.attachVBO(depthVBO, 0, 2, 4 * sizeof(float), 0);
	depthVAO.attachVBO(depthVBO, 1, 2, 4 * sizeof(float), 2 * sizeof(float));
	depthVBO.unbind();

	const char* depthVert = "C:\\Sarthak\\programming\\3dFileLoader\\Editor\\src\\shaders\\depth.vert";
	const char* depthFrag = "C:\\Sarthak\\programming\\3dFileLoader\\Editor\\src\\shaders\\depth.frag";
	ShaderProgram depthShader(depthVert, depthFrag);
	depthShader.setInt("depthTexUnit", 1);
	bool extraVisible = true;
	depthShader.setInt("extraVisible", extraVisible);

	const char* lightPassVert = "C:\\Sarthak\\programming\\3dFileLoader\\Editor\\src\\shaders\\lightPass.vert";
	const char* lightPassFrag = "C:\\Sarthak\\programming\\3dFileLoader\\Editor\\src\\shaders\\lightPass.frag";
	ShaderProgram lightPassShader(lightPassVert, lightPassFrag);

	while (window.running) {

		glEnable(GL_DEPTH_TEST);
		uint32_t curTime = SDL_GetTicks();

		window.pollEvents();
		window.initGuiForFrame();

		/*
			LIGHT CALCULATIONS
		*/
		lightFrameBuffer.bind();
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
		glEnable(GL_DEPTH_TEST);
		glViewport(0, 0, width, height);
		glm::mat4 lightView = lightFrameBuffer.getLightViewMat();
		glm::mat4 lightProj = panelsManager.cameraPanel.getProjectionMat();
		glm::mat4 cameraProj = panelsManager.cameraPanel.getProjectionMat();

		lightPassShader.setMat4("lightView", lightView);
		lightPassShader.setMat4("lightProj", lightProj);
		lightPassShader.bind();
		if (panelsManager.sceneList.curSceneIdx > -1) {
			Scene& scene = panelsManager.sceneList.scenes[panelsManager.sceneList.curSceneIdx];
			std::vector<MeshRenderer>& meshRenderers = panelsManager.sceneList.meshRenderLists[panelsManager.sceneList.curSceneIdx];
			glm::vec3 camPos(panelsManager.cameraPanel.radius * cos(glm::radians(panelsManager.cameraPanel.angle)), panelsManager.cameraPanel.yPos, panelsManager.cameraPanel.radius * sin(glm::radians(panelsManager.cameraPanel.angle)));
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
			SCENE FBO RENDERING
		*/
		glm::mat4 view = panelsManager.cameraPanel.getViewMat();
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

		if (panelsManager.sceneList.curSceneIdx > -1) {
			Scene& scene = panelsManager.sceneList.scenes[panelsManager.sceneList.curSceneIdx];
			std::vector<MeshRenderer>& meshRenderers = panelsManager.sceneList.meshRenderLists[panelsManager.sceneList.curSceneIdx];
			glm::vec3 camPos(panelsManager.cameraPanel.radius * cos(glm::radians(panelsManager.cameraPanel.angle)), panelsManager.cameraPanel.yPos, panelsManager.cameraPanel.radius * sin(glm::radians(panelsManager.cameraPanel.angle)));
			ShaderRegistry& shaderRegistry = panelsManager.shaderRegistry;
			for (int meshId = 0; meshId < scene.numMeshes; meshId++) {

				int shaderIdx = meshRenderers[meshId].shaderIdx;
				shaderRegistry.shaders[shaderIdx].setInt("depthTexUnit", 1);
				shaderRegistry.shaders[shaderIdx].setMat4("lightProj", lightProj);
				shaderRegistry.shaders[shaderIdx].setMat4("lightView", lightView);

				shaderRegistry.shaders[shaderIdx].setMat4("view", view);
				shaderRegistry.shaders[shaderIdx].setMat4("projection", cameraProj);

				shaderRegistry.shaders[shaderIdx].setFloat("light.ambientFactor", light.ambientFactor);
				shaderRegistry.shaders[shaderIdx].setVec3("light.color", light.lightColor);
				shaderRegistry.shaders[shaderIdx].setVec3("light.pos", light.pos);

				shaderRegistry.shaders[shaderIdx].setFloat("material.specularStrength", light.specularFactor);
				shaderRegistry.shaders[shaderIdx].setFloat("material.shininess", light.shininess);

				shaderRegistry.shaders[shaderIdx].setVec3("viewPos", camPos);

				meshRenderers[meshId].render();
			}

			lightCube.shaderProgram.setVec3("color", light.lightColor);
			glm::mat4 translation = glm::translate(glm::mat4(1.0f), light.pos);
			glm::mat4 rotation(1.0f);
			glm::mat4 scale = glm::scale(glm::mat4(1.0f), glm::vec3(100.0f, 100.0f, 100.0f));

			glm::mat4 lightModel = translation * rotation * scale;
			lightCube.shaderProgram.setVec3("color", light.lightColor);
			lightCube.shaderProgram.setMat4("model", lightModel);
			lightCube.shaderProgram.setMat4("view", view);
			lightCube.shaderProgram.setMat4("projection", cameraProj);
			lightCube.render();
		}

		sceneFbo.unbind();

		/*
			IMGUI RENDERING
		*/

		FrameBuffer::ClearBuffers(glm::vec3(0, 0, 0));
		glViewport(0, 0, width, height);

		FileBrowser& fileBrowser = panelsManager.fileBrowser;
		if (ImGui::BeginMainMenuBar()) {

			if (ImGui::BeginMenu("File")) {
				if (ImGui::MenuItem("Open fbx file")) {
					fileBrowser.open = true;
					fileBrowser.resultBuffer = fbxToLoadPath;
					fileBrowser.loadMode = FileBrowserLoadMode::SCENE;
					memset(fileBrowser.curFolderPath, 0, 200);
					// later change default filebrowser path
					Helper::CopyBuffer("C:\\Sarthak\\programming\\3dFileLoader\\Editor\\assets\\3d", fileBrowser.curFolderPath, 200);
					selectingFbxToLoad = true;
				}
				ImGui::EndMenu();
			}
			ImGui::EndMainMenuBar();
		}

		if (selectingFbxToLoad && !fileBrowser.open) {
			selectingFbxToLoad = false;
			panelsManager.sceneList.loadSceneFromFbxFile(fbxToLoadPath);
		}

		glm::vec3 camPos;
		renderDockspace();

		panelsManager.update();
		/*
		meshRenPanel.render();
		sceneHierarchyPanel.render();
		cameraPanel.render();
		shaderEditor.render();
		fileBrowser.render();
		shaderRegistry.render();
		sceneList.render();
		*/

		ImGuiWindowFlags worldViewWinFlags = ImGuiWindowFlags_None;

		ImGuiViewport* mainViewport = ImGui::GetMainViewport();
		ImGui::SetNextWindowViewport(mainViewport->ID);

		ImVec2 sceneViewWinPadding(5.0f, 5.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, sceneViewWinPadding);

		ImGui::SetNextWindowSizeConstraints(ImVec2(100, 100), ImVec2(FLT_MAX, FLT_MAX), setSceneViewWindowConstraint);

		float fontSize = 16.0f;
		ImGui::Begin("World view window", NULL, worldViewWinFlags);
		{
			ImVec2 winSize = ImGui::GetWindowSize();
			ImVec2 actualRenderView(winSize.x - sceneViewWinPadding.x, winSize.y - (2.0f * fontSize) - (2 * sceneViewWinPadding.y));
			ImGui::Image((ImTextureID)sceneFbo.frameBufferTex, actualRenderView, ImVec2(0, 1), ImVec2(1, 0));
			// ImGui::Image((ImTextureID)lightFrameBuffer.colorTexture, actualRenderView, ImVec2(0, 1), ImVec2(1, 0));
			// imgui window is relative from top left and increases going down and to the right
			// opengl is relative from bottom left and increases going up and to the right
			ImVec2 windowPos = ImGui::GetWindowPos();
			ImVec2 windowSize = ImGui::GetWindowSize();

			glm::vec2 openGlPos = glm::vec2(windowPos.x - mainViewport->WorkPos.x, 800 - (windowPos.y - mainViewport->WorkPos.y) - height);
		}

		ImGui::End();
		ImGui::PopStyleVar();

		int windowWidth, windowHeight;
		SDL_GetWindowSize(window.window, &windowWidth, &windowHeight);

		ImGui::SetNextWindowViewport(mainViewport->ID);
		ImGui::SetNextWindowSizeConstraints(ImVec2(100, 100), ImVec2(FLT_MAX, FLT_MAX), setSceneViewWindowConstraint);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
		ImGui::SetWindowPos(ImVec2(0, 0));
		glm::vec4 openGlViewportLightWin;
		ImGui::Begin("Light depth texture");
		{
			ImVec2 lightDepTexWindowSize = ImGui::GetWindowSize();
			ImVec2 lightDepTexWindowPos = ImGui::GetWindowPos();
			ImGuiViewport* mainViewport = ImGui::GetMainViewport();
			ImVec2 realWinCoord;
			realWinCoord.x = lightDepTexWindowPos.x - mainViewport->WorkPos.x;
			realWinCoord.y = lightDepTexWindowPos.y - mainViewport->WorkPos.y;
			openGlViewportLightWin = glm::vec4(realWinCoord.x, windowHeight - realWinCoord.y - lightDepTexWindowSize.y - fontSize, lightDepTexWindowSize.x, lightDepTexWindowSize.y - (2 * fontSize));
		}
		ImGui::End();
		ImGui::PopStyleVar();

		ImGui::Begin("Light editor");
		ImGui::DragFloat3("Light pos", &light.pos.x, 10);
		bool prev = extraVisible;
		ImGui::Checkbox("Light depth buffer extra visible", &extraVisible);
		if (prev != extraVisible) {
			depthShader.setInt("extraVisible", extraVisible);
		}
		ImGui::ColorPicker3("Light color", &light.lightColor.r);
		ImGui::DragFloat("Ambient light factor", &light.ambientFactor, .01, 0, 1);
		ImGui::DragFloat("Specular light factor", &light.specularFactor, .01, 0, 1);
		ImGui::DragFloat("Specular shininess", &light.shininess, 0.5, 5, 64);
		ImGui::End();

		window.renderGui();

		glDisable(GL_DEPTH_TEST);
		glViewport(openGlViewportLightWin.x, openGlViewportLightWin.y, openGlViewportLightWin.z, openGlViewportLightWin.w);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, lightFrameBuffer.depthTexture);
		depthShader.bind();
		depthVAO.bind();
		glDrawArrays(GL_TRIANGLES, 0, 6);
		depthVAO.unbind();
		depthShader.unbind();
		glBindTexture(GL_TEXTURE_2D, 0);

		window.swapBuffers();
	}

	return -1;
}