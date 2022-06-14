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

int width = 600, height = 600;
// Line* linePtr;
NormalRenderer* splitNormalRendererPtr;
NormalRenderer* normalRendererPtr;
ImFont* openSansBold;
ImFont* openSansLight;
MeshRendererSettingsPanel* meshRenPanelPtr;
ShaderEditor* shaderEditorPtr;
FileBrowser* fileBrowserPtr;
ShaderRegistry* shaderRegistryPtr;
SceneList* sceneListPtr;
bool enterPressed = false;

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

	if (SDL_Init(SDL_INIT_EVERYTHING) < 0) {
		std::cout << "sdl gave error" << std::endl;
		return 0;
	}

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
	SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);

	uint32_t windowFlags = SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE;// | SDL_WINDOW_MAXIMIZED;
	SDL_Window* window = SDL_CreateWindow("window", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, windowFlags);

	SDL_GLContext context = SDL_GL_CreateContext(window);

	SDL_GL_MakeCurrent(window, context);
	gladLoadGLLoader(SDL_GL_GetProcAddress);

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
	io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

	ImGui::StyleColorsDark();

	ImGuiStyle& style = ImGui::GetStyle();
	if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		style.WindowRounding = 0.0f;
		style.Colors[ImGuiCol_WindowBg].w = 1.0f;
	}

	ImGui_ImplSDL2_InitForOpenGL(window, context);
	const char* glslVersion = "#version 330";
	ImGui_ImplOpenGL3_Init(glslVersion);

	ShaderRegistry shaderRegistry;
	shaderRegistryPtr = &shaderRegistry;

	glDepthFunc(GL_LESS);
	glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

	bool running = true;

	NormalRenderer splitNormalRenderer;
	splitNormalRendererPtr = &splitNormalRenderer;
	splitNormalRendererPtr->setColor(glm::vec3(1, 0, 0));

	NormalRenderer normalRenderer;
	normalRendererPtr = &normalRenderer;

	glEnable(GL_DEPTH_TEST);

	float fontSize = 16.0f;
	openSansBold = io.Fonts->AddFontFromFileTTF("C:\\Sarthak\\programming\\3dFileLoader\\Editor\\assets\\fonts\\OpenSans-Bold.ttf", fontSize);
	openSansLight = io.Fonts->AddFontFromFileTTF("C:\\Sarthak\\programming\\3dFileLoader\\Editor\\assets\\fonts\\OpenSans-Light.ttf", fontSize);
	io.FontDefault = io.Fonts->AddFontFromFileTTF("C:\\Sarthak\\programming\\3dFileLoader\\Editor\\assets\\fonts\\OpenSans-Regular.ttf", fontSize);

	FrameBuffer sceneFbo;

	MeshRendererSettingsPanel meshRenPanel;
	meshRenPanelPtr = &meshRenPanel;

	SceneHierarchyPanel sceneHierarchyPanel;
	CameraPanel cameraPanel;
	cameraPanel.pov = 45.0f;
	cameraPanel.radius = 2500.0f;
	cameraPanel.angle = 0.0f;
	cameraPanel.yPos = 10.0f;

	FileBrowser fileBrowser;
	fileBrowserPtr = &fileBrowser;

	ShaderEditor shaderEditor;
	shaderEditorPtr = &shaderEditor;

	SceneList sceneList;
	sceneListPtr = &sceneList;

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

	while (running) {

		glEnable(GL_DEPTH_TEST);
		uint32_t curTime = SDL_GetTicks();

		SDL_Event event;
		while (SDL_PollEvent(&event)) {
			enterPressed = false;
			ImGui_ImplSDL2_ProcessEvent(&event);
			if (event.type == SDL_QUIT) {
				running = false;
			}
			if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_CLOSE && event.window.windowID == SDL_GetWindowID(window)) {
				running = false;
			}
			if (event.type == SDL_WINDOWEVENT) {
				if (event.window.event == SDL_WINDOWEVENT_SIZE_CHANGED) {
					// width = event.window.data1;
					// height = event.window.data2;
				}
			}
			if (event.type == SDL_KEYDOWN) {
				SDL_Keycode keyDown = event.key.keysym.sym;
				enterPressed = (keyDown == SDLK_RETURN);
			}
		}

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplSDL2_NewFrame();
		ImGui::NewFrame();

		/*
			LIGHT CALCULATIONS
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
			std::vector<MeshRenderer>& meshRenderers = sceneList.meshRenderLists[sceneList.curSceneIdx];
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
			SCENE FBO RENDERING
		*/
		glm::mat4 view = cameraPanel.getViewMat();
		normalRendererPtr->shaderProgram.setMat4("view", view);
		normalRendererPtr->shaderProgram.setMat4("projection", cameraProj);
		splitNormalRendererPtr->shaderProgram.setMat4("view", view);
		splitNormalRendererPtr->shaderProgram.setMat4("projection", cameraProj);
		// line.shaderProgram.setMat4("view", view);
		// line.shaderProgram.setMat4("projection", proj);

		glViewport(0, 0, width, height);

		sceneFbo.bind();
		FrameBuffer::ClearBuffers(glm::vec3(0, 0, 0));
		glEnable(GL_DEPTH_TEST);
		glViewport(0, 0, width, height);

		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, lightFrameBuffer.depthTexture);

		if (sceneList.curSceneIdx > -1) {
			Scene& scene = sceneList.scenes[sceneList.curSceneIdx];
			std::vector<MeshRenderer>& meshRenderers = sceneList.meshRenderLists[sceneList.curSceneIdx];
			glm::vec3 camPos(cameraPanel.radius * cos(glm::radians(cameraPanel.angle)), cameraPanel.yPos, cameraPanel.radius * sin(glm::radians(cameraPanel.angle)));
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

			// normalRendererPtr->render();
			// splitNormalRendererPtr->render();
		}

		sceneFbo.unbind();

		/*
			IMGUI RENDERING
		*/

		FrameBuffer::ClearBuffers(glm::vec3(0, 0, 0));
		glViewport(0, 0, width, height);

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
			sceneList.loadSceneFromFbxFile(fbxToLoadPath);
		}

		glm::vec3 camPos;
		renderDockspace();

		meshRenPanel.render();
		sceneHierarchyPanel.render();
		cameraPanel.render();
		shaderEditor.render();
		fileBrowser.render();
		shaderRegistry.render();
		sceneList.render();

		ImGuiWindowFlags worldViewWinFlags = ImGuiWindowFlags_None;

		ImGuiViewport* mainViewport = ImGui::GetMainViewport();
		ImGui::SetNextWindowViewport(mainViewport->ID);

		ImVec2 sceneViewWinPadding(5.0f, 5.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, sceneViewWinPadding);

		ImGui::SetNextWindowSizeConstraints(ImVec2(100, 100), ImVec2(FLT_MAX, FLT_MAX), setSceneViewWindowConstraint);

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
		SDL_GetWindowSize(window, &windowWidth, &windowHeight);

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

		glViewport(0, 0, (int)io.DisplaySize.x, (int)io.DisplaySize.y);

		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

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

		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			SDL_Window* backupCurrentWindow = SDL_GL_GetCurrentWindow();
			SDL_GLContext backupCurrentContext = SDL_GL_GetCurrentContext();
			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault();
			SDL_GL_MakeCurrent(backupCurrentWindow, backupCurrentContext);
		}

		SDL_GL_SwapWindow(window);
	}

	return -1;
}