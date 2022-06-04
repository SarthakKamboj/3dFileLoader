#pragma once

#include "glm/glm.hpp"

typedef struct Light {
	float ambientFactor;
	glm::vec3 lightColor;
	glm::vec3 ambientColor;
	glm::vec3 diffuseColor;
	float specularFactor;
	glm::vec3 specularColor;
	glm::vec3 pos;
} Light;
