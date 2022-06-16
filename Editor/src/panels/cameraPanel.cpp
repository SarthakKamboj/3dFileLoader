#include "cameraPanel.h"
#include "glm/gtc/type_ptr.hpp"
#include "imgui.h"
#include "window.h"

float CameraPanel::nearPlane = 10.0f;
float CameraPanel::farPlane = 5000.0f;

glm::mat4 CameraPanel::getProjectionMat() {
	return glm::perspective(glm::radians(pov), ((float)Window::width) / Window::height, CameraPanel::nearPlane, CameraPanel::farPlane);
}

glm::mat4 CameraPanel::getViewMat() {
	glm::vec3 pos(radius * cos(glm::radians(angle)), yPos, radius * sin(glm::radians(angle)));
	return glm::lookAt(pos, glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
}

void CameraPanel::update() {
	ImGui::Begin("Camera panel");

	ImGui::DragFloat("radius", &radius, 10, 10.0f, 2500.0f);
	ImGui::DragFloat("angle", &angle, 1, -180, 180);
	ImGui::DragFloat("yPos", &yPos, 10, -1000.0f, 1000.0f);
	ImGui::DragFloat("pov", &pov, 1, 10.0f, 60.0f);

	ImGui::End();
}
