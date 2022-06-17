#include "helper.h"
#include "glad/glad.h"
#include <iostream>
#include "glm/gtc/type_ptr.hpp"
#include <filesystem>

namespace fs = std::filesystem;

namespace Helper {

	glm::mat4 GetRotationMatrix(const glm::vec3& rot) {
		glm::mat4 rotMatrix(1.0f);
		rotMatrix = glm::rotate(rotMatrix, glm::radians(rot.x), glm::vec3(1, 0, 0));
		rotMatrix = glm::rotate(rotMatrix, glm::radians(rot.y), glm::vec3(0, 1, 0));
		rotMatrix = glm::rotate(rotMatrix, glm::radians(rot.z), glm::vec3(0, 0, 1));
		return rotMatrix;
	}

	int GetLastIndex(const char* buffer, const char c) {
		int res = -1;
		int idx = 0;
		while (*buffer != 0) {
			if (*buffer == c) {
				res = idx;
			}
			buffer++;
			idx++;
		}
		return res;
	}

	int GetLength(const char* buffer) {
		int len = 0;
		while (*buffer != 0) {
			buffer++;
			len++;
		}
		return len;
	}

	bool IsImage(const char* filePath) {
		int lastDot = GetLastIndex(filePath, '.');
		if (lastDot == -1) return false;
		return IsSameString(filePath + lastDot + 1, "png") || IsSameString(filePath + lastDot + 1, "jpg");
	}

	bool Is3dScene(const char* filePath) {
		int lastDot = GetLastIndex(filePath, '.');
		if (lastDot == -1) return false;
		return IsSameString(filePath + lastDot + 1, "fbx");
	}

	void CopyBuffer(const char* srcBuffer, char* destBuffer, int size) {
		for (int i = 0; i < size; i++) {
			destBuffer[i] = srcBuffer[i];
		}
	}

	void ConcatBuffer(char* buffer, const char* extBuffer) {
		while (*buffer != 0) {
			buffer++;
		}
		while (*extBuffer != 0) {
			*buffer = *extBuffer;
			buffer++;
			extBuffer++;
		}
		*buffer = 0;
	}

	bool IsSameString(const char* buffer1, const char* buffer2) {
		while (*buffer1 == *buffer2) {
			if (*buffer1 == 0) {
				return true;
			}
			buffer1++;
			buffer2++;
		}
		return false;
	}

	void CheckFrameBufferStatus() {
		GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
		switch (status) {
		case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT: {
			std::cout << "one of more of the framebuffer attachment points are framebuffer incomplete" << std::endl;
		}
												 break;
		case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT: {
			std::cout << "framebuffer does not have at least one image attached to it" << std::endl;
		}
														 break;
		case GL_FRAMEBUFFER_COMPLETE: {
			std::cout << "framebuffer good to go" << std::endl;
		}
									break;
		}
	}

	void GetApplicationPath(char* bufferToCopyInto) {
		fs::path currentPath = fs::current_path();
		std::string curPathStr = currentPath.string();
		const char* curPathChar = curPathStr.c_str();
		Helper::CopyBuffer(curPathChar, bufferToCopyInto, Helper::GetLength(curPathChar));
		// return curPathChar;
		// CharWrapper charWrapper;
		// charWrapper.str = curPathChar;
		// Helper::GetLastIndex(curPathChar);
		// Helper::CopyBuffer(curPathChar, )
		// charWrapper.strLength = curPathStr.size();
		// return charWrapper;
	}

}