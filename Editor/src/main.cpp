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

/*
static float vertices[] = {
	0.5f, 0.5f, 0.5f,
	0.5f, 0.5f, -0.5f,
	0.5f, -0.5f, 0.5f,
	0.5f, -0.5f, -0.5f,
	-0.5f, 0.5f, 0.5f,
	-0.5f, 0.5f, -0.5f,
	-0.5f, -0.5f, 0.5f,
	-0.5f, -0.5f, -0.5f,
};

static unsigned int indicies[] = {
	0,5,1,0,4,5,
	4,5,6,5,6,7,
	1,0,3,3,2,0,
	7,2,6,7,3,2,
	5,6,7,5,6,1,
	4,3,0,4,3,6
};
*/

float vertices[] = {
		-0.5f, -0.5f, -0.5f,  0.0f, 0.0f, 1.0f,
		 0.5f, -0.5f, -0.5f,  1.0f, 0.0f, 1.0f,
		 0.5f,  0.5f, -0.5f,  1.0f, 1.0f, 1.0f,
		 0.5f,  0.5f, -0.5f,  1.0f, 1.0f, 1.0f,
		-0.5f,  0.5f, -0.5f,  0.0f, 1.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 0.0f, 1.0f,

		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f, 1.0f,
		 0.5f, -0.5f,  0.5f,  1.0f, 0.0f, 1.0f,
		 0.5f,  0.5f,  0.5f,  1.0f, 1.0f, 1.0f,
		 0.5f,  0.5f,  0.5f,  1.0f, 1.0f, 1.0f,
		-0.5f,  0.5f,  0.5f,  0.0f, 1.0f, 1.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f, 1.0f,

		-0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 1.0f,
		-0.5f,  0.5f, -0.5f,  1.0f, 1.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f, 1.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f, 1.0f,
		-0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 1.0f,

		 0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 1.0f,
		 0.5f,  0.5f, -0.5f,  1.0f, 1.0f, 1.0f,
		 0.5f, -0.5f, -0.5f,  0.0f, 1.0f, 1.0f,
		 0.5f, -0.5f, -0.5f,  0.0f, 1.0f, 1.0f,
		 0.5f, -0.5f,  0.5f,  0.0f, 0.0f, 1.0f,
		 0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 1.0f,

		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f, 1.0f,
		 0.5f, -0.5f, -0.5f,  1.0f, 1.0f, 1.0f,
		 0.5f, -0.5f,  0.5f,  1.0f, 0.0f, 1.0f,
		 0.5f, -0.5f,  0.5f,  1.0f, 0.0f, 1.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f, 1.0f,

		-0.5f,  0.5f, -0.5f,  0.0f, 1.0f, 1.0f,
		 0.5f,  0.5f, -0.5f,  1.0f, 1.0f, 1.0f,
		 0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 1.0f,
		 0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 1.0f,
		-0.5f,  0.5f,  0.5f,  0.0f, 0.0f, 1.0f,
		-0.5f,  0.5f, -0.5f,  0.0f, 1.0f, 1.0f
};

glm::mat4 getRotationMatrix(glm::vec3 rot) {
	glm::mat4 rotMatrix(1.0f);
	rotMatrix = glm::rotate(rotMatrix, glm::radians(rot.x), glm::vec3(1, 0, 0));
	rotMatrix = glm::rotate(rotMatrix, glm::radians(rot.y), glm::vec3(0, 1, 0));
	rotMatrix = glm::rotate(rotMatrix, glm::radians(rot.z), glm::vec3(0, 0, 1));
	return rotMatrix;
}

int main(int argc, char* args[]) {

	// const char* fbxFilePath = "C:\\Sarthak\\product_anim\\arrow\\arrow.fbx";
	// const char* fbxFilePath = "C:\\Sarthak\\product_anim\\arrow\\monkey.fbx";
	const char* fbxFilePath = "C:\\Sarthak\\product_anim\\arrow\\triangle.fbx";
	SceneData sceneData = loadFbx(fbxFilePath);

	if (sceneData.meshCount == -1 || !sceneData.meshes) {
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
	ImGui::StyleColorsDark();

	ImGui_ImplSDL2_InitForOpenGL(window, context);
	const char* glsl_version = "#version 330";
	ImGui_ImplOpenGL3_Init(glsl_version);

	glDepthFunc(GL_LESS);
	glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

	bool running = true;

	int numMeshes = sceneData.meshCount;
	// numMeshes = 3;

	std::vector<VAO> vaos;
	std::vector<VBO> vbos;
	// std::vector<EBO> ebos;

	vaos.resize(numMeshes);
	vbos.resize(numMeshes);
	// ebos.resize(numMeshes);

	for (int meshId = 0; meshId < numMeshes; meshId++) {
		Mesh mesh = sceneData.meshes[meshId];

		VAO vao;
		vao.bind();

		VBO vbo;
		vbo.setData((float*)&mesh.vertices[0], mesh.vertexCount * sizeof(Vertex), GL_STATIC_DRAW);
		// vbo.setData(vertices, sizeof(vertices), GL_STATIC_DRAW);

		// vao.attachVBO(vbo, 0, 3, 6, 0);
		// vao.attachVBO(vbo, 1, 3, 6, 3 * sizeof(float));
		int posOffset = offsetof(Vertex, position);
		int colorOffset = offsetof(Vertex, color);
		int uvOffset = offsetof(Vertex, uvs);
		int numFloats = sizeof(Vertex) / sizeof(float);
		vao.attachVBO(vbo, 0, 3, numFloats, offsetof(Vertex, position));
		vao.attachVBO(vbo, 1, 3, numFloats, offsetof(Vertex, color));
		vao.attachVBO(vbo, 2, 2, numFloats, offsetof(Vertex, uvs));

		// vao.attachVBO(vbo, 0, 3, 8, 0);
		// vao.attachVBO(vbo, 1, 3, 8, 3 * sizeof(float));
		// vao.attachVBO(vbo, 2, 2, 8, 6 * sizeof(float));

		// EBO ebo;
		// ebo.setData(mesh.indicies, mesh.indexCount * sizeof(mesh.indicies[0]), GL_STATIC_DRAW);
		// ebo.bind();

		vao.unbind();
		// ebo.unbind();
		vbo.unbind();

		vaos[meshId] = vao;
		vbos[meshId] = vbo;
		// ebos[meshId] = ebo;
	}

	const char* vertexFilePath = "C:\\Sarthak\\programming\\3dFileLoader\\Editor\\src\\shaders\\vertexShader.vert";
	const char* fragmentFilePath = "C:\\Sarthak\\programming\\3dFileLoader\\Editor\\src\\shaders\\fragmentShader.frag";

	ShaderProgram shaderProgram(vertexFilePath, fragmentFilePath);

	glm::vec3 camPos(0, 0, 0);
	glm::mat4 proj = glm::perspective(glm::radians(45.0f), ((float)width) / height, 10.0f, 5000.0f);

	shaderProgram.setMat4("projection", proj);

	int i = 0;

	float radius = 2000.0f;

	stbi_set_flip_vertically_on_load(true);

	const char* texFilePath = "C:\\Sarthak\\programming\\3dFileLoader\\Editor\\src\\images\\images.jpg";
	// const char* texFilePath = "C:\\Sarthak\\programming\\3dFileLoader\\Editor\\src\\images\\images.jpg";
	// const char* texFilePath = "C:\\Sarthak\\programming\\3dFileLoader\\Editor\\src\\images\\images.png";

	int imgWidth, imgHeight, nrChannels;
	unsigned char* data = stbi_load(texFilePath, &imgWidth, &imgHeight, &nrChannels, 0);

	unsigned int texture;
	glGenTextures(1, &texture);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
	// glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	// glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// sets the row alignment to 1, so rows are aligned every 1 byte, which is basically no space between rows
	// glPixelStorei(GL_UNPACK_ALIGNMENT, 8) would set alignment to 8, so rows always start at memory location that is multiple of 8
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	if (nrChannels == 4) {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, imgWidth, imgHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
	}
	else if (nrChannels == 3) {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, imgWidth, imgHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
	}

	glGenerateMipmap(GL_TEXTURE_2D);

	shaderProgram.setInt("texUnit", 0);

	stbi_image_free(data);

	while (running) {
		glViewport(0, 0, width, height);
		glClearColor(0.0f, 1.0f, 0.0f, 1.0f);
		glClearStencil(0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

		SDL_Event event;
		while (SDL_PollEvent(&event)) {
			ImGui_ImplSDL2_ProcessEvent(&event);
			if (event.type == SDL_QUIT) {
				running = false;
			}
		}

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplSDL2_NewFrame();
		ImGui::NewFrame();

		float speed = 0.01f;
		camPos.x = cos(i * speed) * radius;
		camPos.z = sin(i * speed) * radius;
		camPos.y = 0;
		glm::mat4 view = glm::lookAt(camPos, glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
		shaderProgram.setMat4("view", view);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture);
		glEnable(GL_DEPTH_TEST);

		// glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		for (int meshId = 0; meshId < numMeshes; meshId++) {
			Mesh& mesh = sceneData.meshes[meshId];
			glm::mat4 translation = glm::translate(glm::mat4(1.0f), mesh.position);
			glm::mat4 rotation = getRotationMatrix(mesh.rotation);
			glm::mat4 scale = glm::scale(glm::mat4(1.0f), mesh.scale);

			glm::mat4 model = translation * rotation * scale;

			shaderProgram.setMat4("model", model);
			shaderProgram.bind();
			vaos[meshId].bind();
			// glDrawElements(GL_TRIANGLES, mesh.indexCount, GL_UNSIGNED_INT, (void*)0);
			glDrawArrays(GL_TRIANGLES, 0, mesh.vertexCount);
			// glDrawArrays(GL_TRIANGLES, 0, 18);
			vaos[meshId].unbind();
			shaderProgram.unbind();
		}

		// glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

		ImGui::Begin("camPos");

		ImGui::SliderFloat("cam distance", &radius, 0, 4000);

		ImGui::End();

		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		SDL_GL_SwapWindow(window);

		i += 1;
	}

	return -1;
}