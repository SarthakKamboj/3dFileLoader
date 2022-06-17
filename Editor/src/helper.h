#pragma once

#include "glm/glm.hpp"

// Helper functions to manipulated char buffers, evaluate char buffer content, and other miscillaneous things
namespace Helper {

	int GetLastIndex(const char* buffer, const char c);
	void CopyBuffer(const char* srcBuffer, char* destBuffer, int size);
	void ConcatBuffer(char* buffer, const char* extBuffer);
	bool IsSameString(const char* buffer1, const char* buffer2);
	bool IsImage(const char* filePath);
	bool Is3dScene(const char* filePath);
	void CheckFrameBufferStatus();
	glm::mat4 GetRotationMatrix(const glm::vec3& rot);
	void GetApplicationPath(char* bufferToCopyInfo);
	int GetLength(const char* buffer);
}
