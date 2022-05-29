#pragma once

#include "glad/glad.h"
#include "glm/glm.hpp"
#include "glm/gtc/type_ptr.hpp"

class ShaderProgram {
public:
	ShaderProgram();
	ShaderProgram(const char* vertexPath, const char* fragmentPath);
	void setMat4(const char* varName, glm::mat4& mat);
	void setInt(const char* varName, int val);
	void setVec3(const char* varName, glm::vec3 vec3);
	void bind();
	void unbind();

private:
	GLuint programId;
	GLuint createShader(const char* path, GLenum shaderType);
};
