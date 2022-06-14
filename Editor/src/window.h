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
	// renders GUI panels (in this case IMGUI)
	void renderGui();
	// Swaps window screen buffers
	void swapBuffers();
	glm::vec2 getWindowSize();

	Input* input;
	SDL_Window* window;
	SDL_GLContext context;

	int width = 600;
	int height = 600;

	bool running = true;

	static ImFont* openSansBold;
	static ImFont* openSansLight;

private:
	ImGuiIO* ioPtr;
};
