#pragma once

#include "input.h"
#include "SDL.h"
#include "imgui.h"
#include "glm/glm.hpp"

struct Window {
	Window(Input* input);
	// polls for message and inputs
	void pollEvents();
	// readies GUI for current frame
	void initGuiForFrame();
	// Swaps window screen buffers
	void swapBuffers();
	glm::vec2 getWindowSize();

	Input* input;
	SDL_Window* window;
	SDL_GLContext context;

	bool running = true;

	static ImFont* openSansBold;
	static ImFont* openSansLight;

	static int width;
	static int height;

	ImGuiIO* ioPtr;
};
