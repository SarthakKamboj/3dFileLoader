#define ADDING_VULKAN

#include <iostream>
#include "renderer/light.h"
#include "renderer/normalRenderer.h"
#include "window.h"
#include "input.h"
#include "panels/panelsManager.h"
#include "sceneRenderer.h"
#include <stdexcept>
#include "vulkanApp.h"

NormalRenderer* g_SplitNormalRendererPtr;
NormalRenderer* g_NormalRendererPtr;
PanelsManager* g_PanelsManager;
Input* g_Input;
SceneRenderer* g_SceneRenderer;
Window* g_Window;

int main(int argc, char* args[]) {

#ifndef ADDING_VULKAN

	Input input;
	g_Input = &input;
	Window window(g_Input);
	g_Window = &window;

	// will create better system to have multiple lights a little later
	Light light = {};
	light.ambientColor = glm::vec3(1, 0, 0);
	light.ambientFactor = 0.2f;
	light.specularFactor = 0.2f;
	light.pos = glm::vec3(10, 700, 900);
	light.lightColor = glm::vec3(1, 1, 1);
	light.shininess = 32.0f;

	SceneRenderer sceneRenderer(&light);
	g_SceneRenderer = &sceneRenderer;

	// will refactor these NormalRenderers later
	NormalRenderer splitNormalRenderer;
	g_SplitNormalRendererPtr = &splitNormalRenderer;
	g_SplitNormalRendererPtr->setColor(glm::vec3(1, 0, 0));

	NormalRenderer normalRenderer;
	g_NormalRendererPtr = &normalRenderer;

	PanelsManager panelsManager;
	g_PanelsManager = &panelsManager;

	panelsManager.lightEditor.light = &light;

	while (window.running) {

		glEnable(GL_DEPTH_TEST);
		uint32_t curTime = SDL_GetTicks();

		window.pollEvents();
		window.initGuiForFrame();

		sceneRenderer.renderScene();
		panelsManager.update();

		panelsManager.render();

		window.swapBuffers();
	}

#else	
	VulkanApp vulkanApp;
	try {
		vulkanApp.run();
	}
	catch (const std::exception& e) {
		std::cerr << e.what() << std::endl;
		return 1;
	}
#endif

	return -1;
}
