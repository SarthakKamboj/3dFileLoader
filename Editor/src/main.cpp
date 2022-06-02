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

int width = 600, height = 600;
Line* linePtr;
ImFont* openSansBold;
ImFont* openSansLight;
std::vector<MeshRenderer> meshRenderers;
MeshRendererSettingsPanel* meshRenPanelPtr;
Scene* scenePtr;
ShaderEditor* shaderEditorPtr;
FileBrowser* fileBrowserPtr;
ShaderRegistry* shaderRegistryPtr;
bool enterPressed = false;

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

/*
struct Test {

	Test() {
		x = 0;
		y = 0;
		std::cout << "this: " << this << std::endl;
		std::cout << "&x: " << &this->x << std::endl;
		std::cout << "&y: " << &this->y << std::endl;
	}

	Test(int _x, int _y) {
		x = _x;
		y = _y;
		std::cout << "this: " << this << std::endl;
		std::cout << "&x: " << &this->x << std::endl;
		std::cout << "&y: " << &this->y << std::endl;
	}

	int x;
	int y;
};
*/

int main(int argc, char* args[]) {

	/*
	Test t;
	std::cout << "&t: " << &t << std::endl;
	std::cout << "&t.x " << &t.x << std::endl;
	std::cout << "&t.y " << &t.y << std::endl;
	int p1 = 5;
	int p2 = 5;
	std::cout << "&p1: " << &p1 << std::endl;
	std::cout << "&p2: " << &p2 << std::endl;
	Test newT = Test(p1, p2);
	std::cout << "&newT: " << &newT << std::endl;
	t = newT;
	std::cout << "&t: " << &t << std::endl;
	std::cout << "&t.x " << &t.x << std::endl;
	std::cout << "&t.y " << &t.y << std::endl;
	*/

	// const char* fbxFilePath = "C:\\Sarthak\\product_anim\\arrow\\arrow.fbx";
	const char* fbxFilePath = "C:\\Sarthak\\programming\\3dFileLoader\\Editor\\assets\\3d\\texturesTest.fbx";
	// const char* fbxFilePath = "C:\\Sarthak\\product_anim\\arrow\\scene.fbx";
	// const char* fbxFilePath = "C:\\Sarthak\\product_anim\\arrow\\cone.fbx";
	// const char* fbxFilePath = "C:\\Sarthak\\product_anim\\arrow\\pyramid.fbx";
	// const char* fbxFilePath = "C:\\Sarthak\\product_anim\\arrow\\monkey.fbx";
	// const char* fbxFilePath = "C:\\Sarthak\\product_anim\\arrow\\triangle.fbx";
	// const char* fbxFilePath = "C:\\Sarthak\\product_anim\\arrow\\cube.fbx";
	// const char* fbxFilePath = "..\\assets\\3d\\parentTest.fbx";
	// const char* fbxFilePath = "C:\\Sarthak\\programming\\3dFileLoader\\Editor\\assets\\3d\\parentTest.fbx";
	Scene scene = loadFbx(fbxFilePath);
	scenePtr = &scene;

	if (scene.numMeshes == -1) {
		std::cout << "scene data not valid" << std::endl;
		return 0;
	}

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

	int numMeshes = scene.numMeshes;
	meshRenderers.resize(numMeshes);

	for (int meshId = 0; meshId < numMeshes; meshId++) {
		Mesh& mesh = scene.meshes[meshId];
		meshRenderers[meshId] = MeshRenderer(&mesh);
	}

	const char* texFilePath = "C:\\Sarthak\\programming\\3dFileLoader\\Editor\\src\\images\\arrow.png";
	Texture texture(texFilePath, 0);

	Line line;
	linePtr = &line;

	glEnable(GL_DEPTH_TEST);

	float fontSize = 16.0f;
	openSansBold = io.Fonts->AddFontFromFileTTF("assets/fonts/OpenSans-Bold.ttf", fontSize);
	openSansLight = io.Fonts->AddFontFromFileTTF("assets/fonts/OpenSans-Light.ttf", fontSize);
	io.FontDefault = io.Fonts->AddFontFromFileTTF("assets/fonts/OpenSans-Regular.ttf", fontSize);

	FrameBuffer sceneFbo;
	MeshRendererSettingsPanel meshRenPanel;
	meshRenPanelPtr = &meshRenPanel;

	SceneHierarchyPanel sceneHierarchyPanel(scene);
	CameraPanel cameraPanel;
	cameraPanel.pov = 45.0f;
	cameraPanel.radius = 1000.0f;
	cameraPanel.angle = 0.0f;
	cameraPanel.yPos = 10.0f;

	FileBrowser fileBrowser;
	fileBrowserPtr = &fileBrowser;

	ShaderEditor shaderEditor;
	shaderEditorPtr = &shaderEditor;

	while (running) {

		uint32_t curTime = SDL_GetTicks();

		FrameBuffer::ClearBuffers(glm::vec3(0, 0, 0));
		glViewport(0, 0, width, height);

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
			if (event.type == SDL_KEYDOWN) {
				SDL_Keycode keyDown = event.key.keysym.sym;
				enterPressed = (keyDown == SDLK_RETURN);
			}
		}

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplSDL2_NewFrame();
		ImGui::NewFrame();

		glm::vec3 camPos;
		renderDockspace();

		meshRenPanel.render();
		sceneHierarchyPanel.render();
		cameraPanel.render();
		shaderEditor.render();
		fileBrowser.render();
		shaderRegistry.render();

		ImGuiWindowFlags worldViewWinFlags = ImGuiWindowFlags_None;

		ImGuiViewport* mainViewport = ImGui::GetMainViewport();
		ImGui::SetNextWindowViewport(mainViewport->ID);

		ImVec2 sceneViewWinPadding(5.0f, 5.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, sceneViewWinPadding);

		ImGui::SetNextWindowSizeConstraints(ImVec2(100, 100), ImVec2(FLT_MAX, FLT_MAX), setSceneViewWindowConstraint);

		ImGui::Begin("World view window", NULL, worldViewWinFlags);
		ImVec2 winSize = ImGui::GetWindowSize();
		ImVec2 actualRenderView(winSize.x - sceneViewWinPadding.x, winSize.y - (2.0f * fontSize) - (2 * sceneViewWinPadding.y));
		ImGui::Image((ImTextureID)sceneFbo.frameBufferTex, actualRenderView, ImVec2(0, 1), ImVec2(1, 0));
		// imgui window is relative from top left and increases going down
		// opengl is relative from bottom left and increases going up
		ImVec2 windowPos = ImGui::GetWindowPos();
		ImVec2 windowSize = ImGui::GetWindowSize();

		glm::vec2 openGlPos = glm::vec2(windowPos.x - mainViewport->WorkPos.x, 800 - (windowPos.y - mainViewport->WorkPos.y) - height);

		ImGui::End();
		ImGui::PopStyleVar();

		glViewport(0, 0, (int)io.DisplaySize.x, (int)io.DisplaySize.y);

		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		glViewport(0, 0, width, height);
		sceneFbo.bind();
		FrameBuffer::ClearBuffers(glm::vec3(0, 0, 0));
		glEnable(GL_DEPTH_TEST);

		float speed = 0.01f;
		float radius = 3000.0f;
		camPos.x = cos(curTime * 0.05f * speed) * radius;
		camPos.z = sin(curTime * 0.05f * speed) * radius;
		camPos.y = 0;

		glm::mat4 view = cameraPanel.getViewMat();
		glm::mat4 proj = cameraPanel.getProjectionMat();
		line.shaderProgram.setMat4("view", view);
		line.shaderProgram.setMat4("projection", proj);

		texture.bind();

		for (int meshId = 0; meshId < numMeshes; meshId++) {
			meshRenderers[meshId].shaderProgram.setMat4("view", view);
			meshRenderers[meshId].shaderProgram.setMat4("projection", proj);
			meshRenderers[meshId].render();
		}

		sceneFbo.unbind();

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