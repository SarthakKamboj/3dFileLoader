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
#include <vector>
#include "imgui.h"
#include "imgui_impl_opengl3.h"
#include "imgui_impl_sdl.h"
#include "stb_image.h"
#include "renderer/texture.h"
#include "renderer/line.h"

glm::mat4 getRotationMatrix(glm::vec3 rot) {
	glm::mat4 rotMatrix(1.0f);
	rotMatrix = glm::rotate(rotMatrix, glm::radians(rot.x), glm::vec3(1, 0, 0));
	rotMatrix = glm::rotate(rotMatrix, glm::radians(rot.y), glm::vec3(0, 1, 0));
	rotMatrix = glm::rotate(rotMatrix, glm::radians(rot.z), glm::vec3(0, 0, 1));
	return rotMatrix;
}

int main(int argc, char* args[]) {

	// const char* fbxFilePath = "C:\\Sarthak\\product_anim\\arrow\\arrow.fbx";
	const char* fbxFilePath = "C:\\Sarthak\\product_anim\\arrow\\scene.fbx";
	// const char* fbxFilePath = "C:\\Sarthak\\product_anim\\arrow\\cone.fbx";
	// const char* fbxFilePath = "C:\\Sarthak\\product_anim\\arrow\\pyramid.fbx";
	// const char* fbxFilePath = "C:\\Sarthak\\product_anim\\arrow\\monkey.fbx";
	// const char* fbxFilePath = "C:\\Sarthak\\product_anim\\arrow\\triangle.fbx";
	// const char* fbxFilePath = "C:\\Sarthak\\product_anim\\arrow\\cube.fbx";
	Scene scene = loadFbx(fbxFilePath);

	if (scene.numMeshes == -1) {
		std::cout << "scene data not valid" << std::endl;
		return 0;
	}

	if (SDL_Init(SDL_INIT_EVERYTHING) < 0) {
		std::cout << "sdl gave error" << std::endl;
		return 0;
	}

	int width = 800, height = 800;

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
	SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);

	SDL_Window* window = SDL_CreateWindow("window", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL);
	SDL_GLContext context = SDL_GL_CreateContext(window);

	SDL_GL_MakeCurrent(window, context);
	gladLoadGLLoader(SDL_GL_GetProcAddress);

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;       // Enable Keyboard Controls
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking
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

	glDepthFunc(GL_LESS);
	glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

	bool running = true;

	int numMeshes = scene.numMeshes;

	std::vector<VAO> vaos;
	std::vector<VBO> vbos;

	vaos.resize(numMeshes);
	vbos.resize(numMeshes);

	for (int meshId = 0; meshId < numMeshes; meshId++) {
		Mesh mesh = scene.meshes[meshId];

		VAO vao;
		vao.bind();

		VBO vbo;
		vbo.setData((float*)&mesh.vertices[0], mesh.vertexCount * sizeof(Vertex), GL_STATIC_DRAW);

		vao.attachVBO(vbo, 0, 3, sizeof(Vertex), offsetof(Vertex, position));
		vao.attachVBO(vbo, 1, 3, sizeof(Vertex), offsetof(Vertex, color));
		vao.attachVBO(vbo, 2, 2, sizeof(Vertex), offsetof(Vertex, uvs));
		vao.attachVBO(vbo, 3, 3, sizeof(Vertex), offsetof(Vertex, avgNormal));

		vao.unbind();
		vbo.unbind();

		vaos[meshId] = vao;
		vbos[meshId] = vbo;
	}

	const char* vertexFilePath = "C:\\Sarthak\\programming\\3dFileLoader\\Editor\\src\\shaders\\vertexShader.vert";
	const char* fragmentFilePath = "C:\\Sarthak\\programming\\3dFileLoader\\Editor\\src\\shaders\\fragmentShader.frag";

	ShaderProgram shaderProgram(vertexFilePath, fragmentFilePath);

	glm::mat4 proj = glm::perspective(glm::radians(45.0f), ((float)width) / height, 10.0f, 5000.0f);

	shaderProgram.setMat4("projection", proj);

	shaderProgram.setInt("texUnit", 0);

	const char* texFilePath = "C:\\Sarthak\\programming\\3dFileLoader\\Editor\\src\\images\\arrow.png";
	Texture texture(texFilePath, 0);

	Line line;
	line.shaderProgram.setMat4("projection", proj);

	// glm::vec3 camPos(500.0f, 0, 500.0f);

	glEnable(GL_DEPTH_TEST);

	float fontSize = 16.0f;
	io.Fonts->AddFontFromFileTTF("assets/fonts/OpenSans-Bold.ttf", fontSize);
	io.FontDefault = io.Fonts->AddFontFromFileTTF("assets/fonts/OpenSans-Regular.ttf", fontSize);

	float radius = 500.0f;
	glm::vec3 camPos(radius, 10.0f, radius);
	bool open = true;
	while (running) {
		glViewport(0, 0, width, height);
		glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
		glClearStencil(0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

		SDL_Event event;
		while (SDL_PollEvent(&event)) {
			ImGui_ImplSDL2_ProcessEvent(&event);
			if (event.type == SDL_QUIT) {
				running = false;
			}
			if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_CLOSE && event.window.windowID == SDL_GetWindowID(window)) {
				running = false;
			}
		}

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplSDL2_NewFrame();
		ImGui::NewFrame();

		if (open) {
			ImGuiWindowFlags windowFlags = ImGuiWindowFlags_None;

			ImGuiWindowFlags_;

			windowFlags |= ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
			windowFlags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus | ImGuiWindowFlags_NoTitleBar;

			ImGuiViewport* mainViewport = ImGui::GetMainViewport();
			ImGui::SetNextWindowPos(mainViewport->WorkPos);
			ImGui::SetNextWindowSize(ImVec2(mainViewport->WorkSize.x, mainViewport->WorkSize.y));
			ImGui::SetNextWindowViewport(mainViewport->ID);

			ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));

			ImGui::Begin("Test", NULL, windowFlags);

			ImGuiIO& io = ImGui::GetIO();
			ImGuiDockNodeFlags dockspaceFlags = ImGuiDockNodeFlags_None;
			if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable) {
				ImGuiID id = ImGui::GetID("Dock");
				ImGui::DockSpace(id, ImVec2(0, 0), dockspaceFlags);
			}

			ImGui::End();
			ImGui::PopStyleVar(3);
		}

		ImGui::Begin("camPos");
		ImGui::DragFloat3("cam pos", &camPos.x, 10, -4000, 4000);
		ImGui::End();

		ImGui::ShowDemoWindow();

		ImGuiWindowFlags worldViewWinFlags = ImGuiWindowFlags_None;

		ImGuiViewport* mainViewport = ImGui::GetMainViewport();
		ImGui::SetNextWindowViewport(mainViewport->ID);

		ImVec2 sceneViewWinPadding(0.0f, 5.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, sceneViewWinPadding);
		ImGui::Begin("World view window", NULL, worldViewWinFlags);

		// imgui window is relative from top left and increases going down
		// opengl is relative from bottom left and increases going up
		ImVec2 windowPos = ImGui::GetWindowPos();
		ImVec2 windowSize = ImGui::GetWindowSize();

		glm::vec2 openGlPos = glm::vec2(windowPos.x - mainViewport->WorkPos.x, 800 - (windowPos.y - mainViewport->WorkPos.y) - windowSize.y);

		ImGui::End();
		ImGui::PopStyleVar();

		glViewport(0, 0, (int)io.DisplaySize.x, (int)io.DisplaySize.y);

		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		glViewport(openGlPos.x, openGlPos.y, windowSize.x, windowSize.y - fontSize - sceneViewWinPadding.y);

		float speed = 0.01f;
		// camPos.x = cos(i * speed) * radius;
		// camPos.z = sin(i * speed) * radius;
		// camPos.y = 10;
		glm::mat4 view = glm::lookAt(camPos, glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
		shaderProgram.setMat4("view", view);
		line.shaderProgram.setMat4("view", view);

		texture.bind();

		// glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		for (int meshId = 0; meshId < numMeshes; meshId++) {
			Mesh& mesh = scene.meshes[meshId];
			glm::mat4 translation = glm::translate(glm::mat4(1.0f), mesh.transform.position);
			glm::mat4 rotation = getRotationMatrix(mesh.transform.rotation);
			glm::mat4 scale = glm::scale(glm::mat4(1.0f), mesh.transform.scale);

			glm::mat4 model = translation * rotation * scale;

			/*
			line.shaderProgram.setMat4("model", model);
			for (int vertexIdx = 0; vertexIdx < mesh.vertexCount; vertexIdx++) {
				Vertex& vert = mesh.vertices[vertexIdx];
				glm::vec3 startPoint(vert.position[0], vert.position[1], vert.position[2]);
				glm::vec3 endPoint = startPoint + 100.0f * glm::vec3(vert.normal[0], vert.normal[1], vert.normal[2]);
				glm::vec3 avgEndPoint = startPoint + 1.25f * 100.0f * glm::vec3(vert.avgNormal[0], vert.avgNormal[1], vert.avgNormal[2]);
				line.setStartPoint(startPoint);

				line.setColor(glm::vec3(1, 0, 0));
				line.setEndPoint(endPoint);
				line.render();

				line.setColor(glm::vec3(0, 0, 1));
				line.setEndPoint(avgEndPoint);
				line.render();

			}
			*/

			shaderProgram.setMat4("model", model);
			shaderProgram.bind();
			vaos[meshId].bind();
			glDrawArrays(GL_TRIANGLES, 0, mesh.vertexCount);
			vaos[meshId].unbind();
			shaderProgram.unbind();
		}
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

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