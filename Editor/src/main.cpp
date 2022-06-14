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
#include "sceneRenderer.h"

int width = 600, height = 600;
NormalRenderer* splitNormalRendererPtr;
NormalRenderer* normalRendererPtr;
PanelsManager* g_PanelsManager;
Input* g_Input;
SceneRenderer* g_SceneRenderer;
Window* g_Window;

extern float nearPlane, farPlane;


glm::mat4 getRotationMatrix(glm::vec3 rot) {
	glm::mat4 rotMatrix(1.0f);
	rotMatrix = glm::rotate(rotMatrix, glm::radians(rot.x), glm::vec3(1, 0, 0));
	rotMatrix = glm::rotate(rotMatrix, glm::radians(rot.y), glm::vec3(0, 1, 0));
	rotMatrix = glm::rotate(rotMatrix, glm::radians(rot.z), glm::vec3(0, 0, 1));
	return rotMatrix;
}

int main(int argc, char* args[]) {

	Input input;
	g_Input = &input;
	Window window(g_Input);
	g_Window = &window;

	Light light = {};
	light.ambientColor = glm::vec3(1, 0, 0);
	light.ambientFactor = 0.2f;
	light.specularFactor = 0.2f;
	light.pos = glm::vec3(10, 700, 900);
	light.lightColor = glm::vec3(0, 1, 0);
	light.shininess = 32.0f;

	SceneRenderer sceneRenderer(&light);
	g_SceneRenderer = &sceneRenderer;

	NormalRenderer splitNormalRenderer;
	splitNormalRendererPtr = &splitNormalRenderer;
	splitNormalRendererPtr->setColor(glm::vec3(1, 0, 0));

	NormalRenderer normalRenderer;
	normalRendererPtr = &normalRenderer;

	PanelsManager panelsManager;
	g_PanelsManager = &panelsManager;

	panelsManager.lightEditor.light = &light;

	char fbxToLoadPath[300] = {};
	bool selectingFbxToLoad = false;

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

		sceneRenderer.renderScene();

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

		window.renderGui();

		window.swapBuffers();
	}

	return -1;
}