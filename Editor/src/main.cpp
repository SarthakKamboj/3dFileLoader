#include <iostream>
#include "renderer/light.h"
#include "renderer/normalRenderer.h"
#include "window.h"
#include "input.h"
#include "panels/panelsManager.h"
#include "sceneRenderer.h"

// int width = 600, height = 600;
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
	splitNormalRendererPtr = &splitNormalRenderer;
	splitNormalRendererPtr->setColor(glm::vec3(1, 0, 0));

	NormalRenderer normalRenderer;
	normalRendererPtr = &normalRenderer;

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
		window.renderGui();

		window.swapBuffers();
	}

	return -1;
}