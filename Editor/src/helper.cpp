#include "helper.h"
#include "glad/glad.h"
#include <iostream>

namespace Helper {
	int GetLastIndex(const char* buffer, char c) {
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

	void ConcatBuffer(char* buffer, char* extBuffer) {
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
}