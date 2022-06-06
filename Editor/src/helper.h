#pragma once

namespace Helper {
	int GetLastIndex(const char* buffer, char c);
	void CopyBuffer(const char* srcBuffer, char* destBuffer, int size);
	void ConcatBuffer(char* buffer, char* extBuffer);
	bool IsSameString(const char* buffer1, const char* buffer2);
	bool IsImage(const char* filePath);
	bool Is3dScene(const char* filePath);
	void CheckFrameBufferStatus();
}
