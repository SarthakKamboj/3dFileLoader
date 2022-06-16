#pragma once

#include "glm/glm.hpp"

struct Light {
	float ambientFactor;
	glm::vec3 lightColor;
	glm::vec3 ambientColor;
	float specularFactor;
	glm::vec3 pos;
	float shininess;
};
