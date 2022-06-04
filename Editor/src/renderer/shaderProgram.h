#pragma once

#include "glad/glad.h"
#include "glm/glm.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "texture.h"

struct ShaderProgram {
	ShaderProgram();
	ShaderProgram(const char* vertexPath, const char* fragmentPath);
	void setMat4(const char* varName, glm::mat4& mat);
	void setInt(const char* varName, int val);
	void setFloat(const char* varName, float val);
	void setVec3(const char* varName, glm::vec3 vec3);
	void bind();
	void unbind();

	GLuint programId;
	GLuint createShader(const char* path, GLenum shaderType);

	static int shaderId;
	char name[50];
	glm::vec3 color = glm::vec3(1.0f, 1.0f, 1.0f);
	bool textureBasedColor = false;
	Texture texture;
	float normalDisplacement;
	// char texturePath[200] = {};
};
