#include "cameraPanel.h"
#include "glm/gtc/type_ptr.hpp"
#include "imgui.h"

extern int width, height;
float nearPlane = 10.0f;
float farPlane = 5000.0f;

glm::mat4 CameraPanel::getProjectionMat() {
	return glm::perspective(glm::radians(pov), ((float)width) / height, nearPlane, farPlane);
}

glm::mat4 CameraPanel::getViewMat() {
	glm::vec3 pos(radius * cos(glm::radians(angle)), yPos, radius * sin(glm::radians(angle)));
	return glm::lookAt(pos, glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
}

void CameraPanel::render() {
	ImGui::Begin("Camera panel");

	ImGui::DragFloat("radius", &radius, 10, 10.0f, 2500.0f);
	ImGui::DragFloat("angle", &angle, 1, -180, 180);
	ImGui::DragFloat("yPos", &yPos, 10, -1000.0f, 1000.0f);
	ImGui::DragFloat("pov", &pov, 1, 10.0f, 60.0f);

	ImGui::End();
}
