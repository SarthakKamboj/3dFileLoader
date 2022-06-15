#pragma once

#include "glm/glm.hpp"
#include "fbxLoader.h"

struct CameraPanel {
	float radius;
	float angle;
	float yPos;
	glm::vec3 lookAt;
	float pov;

	glm::mat4 getProjectionMat();
	glm::mat4 getViewMat();

	void update();
};

