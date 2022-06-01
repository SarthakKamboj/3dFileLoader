#include "cameraPanel.h"
#include "glm/gtc/type_ptr.hpp"
#include "imgui.h"

extern int width, height;

glm::mat4 CameraPanel::getProjectionMat() {
	return glm::perspective(glm::radians(pov), ((float)width) / height, 10.0f, 5000.0f);
}

glm::mat4 CameraPanel::getViewMat() {
	return glm::lookAt(transform.position, glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
}

void CameraPanel::render() {
	ImGui::Begin("Camera panel");

	ImGui::DragFloat3("position", &transform.position.x, 10, -2000, 2000);
	ImGui::DragFloat("pov", &pov, 1, 10.0f, 60.0f);

	ImGui::End();
}
