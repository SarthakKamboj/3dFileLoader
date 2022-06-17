#include "shaderProgram.h"
#include <fstream>
#include <string>
#include <iostream>
#include "helper.h"

int ShaderProgram::shaderId = 0;

ShaderProgram::ShaderProgram() {
	programId = -1;
	ShaderProgram::shaderId += 1;

	normalDisplacement = 0;

	// create default texture
	/*
	char defaultTexPath[200] = {};
	Helper::GetApplicationPath(defaultTexPath);
	const char* relativeImgPath = "\\src\\images\\images.jpg";
	Helper::ConcatBuffer(defaultTexPath, relativeImgPath);
	*/

	const char* defaultTexPath = "C:\\Sarthak\\programming\\3dFileLoader\\Editor\\src\\images\\images.jpg";
	texture = Texture(defaultTexPath, 0);
}

ShaderProgram::ShaderProgram(const char* vertexPath, const char* fragmentPath) {
	normalDisplacement = 0;
	// create vertex and fragment shaders
	GLuint vertexId = createShader(vertexPath, GL_VERTEX_SHADER);
	sprintf_s(name, "Shader_%i", ShaderProgram::shaderId);
	if (vertexId == -1) {
		programId = -1;
		return;
	}
	GLuint fragmentId = createShader(fragmentPath, GL_FRAGMENT_SHADER);
	if (fragmentId == -1) {
		programId = -1;
		return;
	}
	programId = glCreateProgram();
	glAttachShader(programId, vertexId);
	glAttachShader(programId, fragmentId);
	glLinkProgram(programId);

	glDeleteShader(vertexId);
	glDeleteShader(fragmentId);

	// create default texture
	const char* defaultTexPath = "C:\\Sarthak\\programming\\3dFileLoader\\Editor\\src\\images\\uv_mapper.jpg";
	texture = Texture(defaultTexPath, 0);
}

void ShaderProgram::bind() {
	glUseProgram(programId);
}

void ShaderProgram::unbind() {
	glUseProgram(0);
}

void ShaderProgram::setMat4(const char* varName, const glm::mat4& mat) {
	if (programId == -1) return;
	bind();
	GLint loc = glGetUniformLocation(programId, varName);
	if (loc == -1) {
		std::cout << varName << " doesn't exist" << std::endl;
	}
	glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(mat));
	unbind();
}

void ShaderProgram::setFloat(const char* varName, float val) {
	if (programId == -1) return;
	bind();
	GLint loc = glGetUniformLocation(programId, varName);
	if (loc == -1) {
		std::cout << varName << " doesn't exist" << std::endl;
	}
	glUniform1f(loc, val);
	unbind();
}

void ShaderProgram::setVec3(const char* varName, const glm::vec3& vec3) {
	if (programId == -1) return;
	bind();
	GLint loc = glGetUniformLocation(programId, varName);
	if (loc == -1) {
		std::cout << varName << " doesn't exist" << std::endl;
	}
	glUniform3fv(loc, 1, glm::value_ptr(vec3));
	unbind();
}

void ShaderProgram::setInt(const char* varName, int val) {
	if (programId == -1) return;
	bind();
	GLint loc = glGetUniformLocation(programId, varName);
	if (loc == -1) {
		std::cout << varName << " doesn't exist" << std::endl;
	}
	glUniform1i(loc, val);
	unbind();
}

GLuint ShaderProgram::createShader(const char* path, GLenum shaderType) {
	// get shader file contents
	std::string line, fileContents;
	std::ifstream file(path);

	while (std::getline(file, line)) {
		fileContents += line + "\n";
	}

	// compile shader and make sure there aren't any errors other return shader id
	const char* fileCodeChar = fileContents.c_str();
	GLuint shaderId = glCreateShader(shaderType);
	glShaderSource(shaderId, 1, &fileCodeChar, NULL);
	glCompileShader(shaderId);

	GLint success = 0;
	glGetShaderiv(shaderId, GL_COMPILE_STATUS, &success);

	if (success == GL_FALSE) {
		std::cout << (shaderType == GL_VERTEX_SHADER ? "vertex" : "fragment") << " shader compilation failed" << std::endl;
		int maxErrorLength = 1024;
		char errorInfo[1024];
		glGetShaderInfoLog(shaderId, maxErrorLength, &maxErrorLength, errorInfo);
		std::cout << "ERROR: " << errorInfo << std::endl;
		return -1;
	}

	return shaderId;
}
