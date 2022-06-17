#include "lightEditor.h"
#include "window.h"
#include "imgui.h"
#include "sceneRenderer.h"
#include "glad/glad.h"
#include "helper.h"

extern Window* g_Window;
extern SceneRenderer* g_SceneRenderer;

static float quadVertices[] = {
	// positions   // texCoords
	-1.0f,  1.0f,  0.0f, 1.0f,
	-1.0f, -1.0f,  0.0f, 0.0f,
	 1.0f, -1.0f,  1.0f, 0.0f,

	-1.0f,  1.0f,  0.0f, 1.0f,
	 1.0f, -1.0f,  1.0f, 0.0f,
	 1.0f,  1.0f,  1.0f, 1.0f
};

LightEditor::LightEditor() {
	// get shader paths
	/*
	char depthVert[200] = {};
	char depthFrag[200] = {};
	Helper::GetApplicationPath(depthVert);
	Helper::GetApplicationPath(depthFrag);
	const char* relativeShadersFolderPath = "\\src\\shaders\\";
	Helper::ConcatBuffer(depthVert, relativeShadersFolderPath);
	Helper::ConcatBuffer(depthFrag, relativeShadersFolderPath);
	Helper::ConcatBuffer(depthVert, "depth.vert");
	Helper::ConcatBuffer(depthFrag, "depth.frag");
	*/
	const char* depthVert = "C:\\Sarthak\\programming\\3dFileLoader\\Editor\\src\\shaders\\depth.vert";
	const char* depthFrag = "C:\\Sarthak\\programming\\3dFileLoader\\Editor\\src\\shaders\\depth.frag";

	// create depth shader program
	depthShader = ShaderProgram(depthVert, depthFrag);
	depthShader.setInt("depthTexUnit", 1);
	depthShader.setInt("extraVisible", extraVisible);

	depthVBO.bind();
	depthVBO.setData(quadVertices, sizeof(quadVertices), GL_STATIC_DRAW);
	depthVAO.bind();
	depthVAO.attachVBO(depthVBO, 0, 2, 4 * sizeof(float), 0);
	depthVAO.attachVBO(depthVBO, 1, 2, 4 * sizeof(float), 2 * sizeof(float));
	depthVBO.unbind();

	testTexture = Texture("C:\\Sarthak\\programming\\3dFileLoader\\Editor\\src\\images\\suzanneTexture.png", 1);
}

void LightEditor::update() {

	glm::vec2 windowSize = g_Window->getWindowSize();

	ImGuiViewport* mainViewport = ImGui::GetMainViewport();
	ImGui::SetNextWindowViewport(mainViewport->ID);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
	ImGui::SetWindowPos(ImVec2(0, 0));
	float fontSize = 16.0f;
	ImGui::Begin("Light depth texture");
	{
		// get size and position of this ImGui window
		ImVec2 lightDepTexWindowSize = ImGui::GetWindowSize();
		ImVec2 lightDepTexWindowPos = ImGui::GetWindowPos();
		ImGuiViewport* mainViewport = ImGui::GetMainViewport();
		ImVec2 realWinCoord;
		realWinCoord.x = lightDepTexWindowPos.x - mainViewport->WorkPos.x;
		realWinCoord.y = lightDepTexWindowPos.y - mainViewport->WorkPos.y;
		openGlViewportLightWin = glm::vec4(realWinCoord.x, windowSize.y - realWinCoord.y - lightDepTexWindowSize.y - fontSize, lightDepTexWindowSize.x, lightDepTexWindowSize.y - (2 * fontSize));
	}
	ImGui::End();
	ImGui::PopStyleVar();

	ImGui::Begin("Light editor");
	Light* light = g_SceneRenderer->lightFrameBuffer.light;
	ImGui::DragFloat3("Light pos", &light->pos.x, 10);

	// toggle whether to display regular light depth texture or make it more visible
	bool prev = extraVisible;
	ImGui::Checkbox("Light depth buffer extra visible", &extraVisible);
	if (prev != extraVisible) {
		depthShader.setInt("extraVisible", extraVisible);
	}

	ImGui::ColorPicker3("Light color", &light->lightColor.r);
	ImGui::DragFloat("Ambient light factor", &light->ambientFactor, .01, 0, 1);
	ImGui::DragFloat("Specular light factor", &light->specularFactor, .01, 0, 1);
	ImGui::DragFloat("Specular shininess", &light->shininess, 0.5, 5, 64);
	ImGui::End();
}

void LightEditor::render() {
	// render the light depth texture in the "Light depth texture" window
	glDisable(GL_DEPTH_TEST);
	glViewport(openGlViewportLightWin.x, openGlViewportLightWin.y, openGlViewportLightWin.z, openGlViewportLightWin.w);

	glActiveTexture(GL_TEXTURE1);
	// glBindTexture(GL_TEXTURE_2D, g_SceneRenderer->lightFrameBuffer.depthTexture);
	glBindTexture(GL_TEXTURE_2D, testTexture.texture);

	depthShader.bind();
	depthVAO.bind();
	glDrawArrays(GL_TRIANGLES, 0, 6);
	depthVAO.unbind();
	depthShader.unbind();
	glBindTexture(GL_TEXTURE_2D, 0);

	glEnable(GL_DEPTH_TEST);
}