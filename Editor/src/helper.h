#pragma once

// Helper functions to manipulated char buffers, evaluate char buffer content, and other miscillaneous things
namespace Helper {
	/*
	Get the last index of a const char buffer
	*/
	int GetLastIndex(const char* buffer, char c);
	/*
	Copy char data from a source buffer to a destination buffer of size bytes
	*/
	void CopyBuffer(const char* srcBuffer, char* destBuffer, int size);
	/*
	Append extBuffer of chars to buffer
	*/
	void ConcatBuffer(char* buffer, char* extBuffer);
	/*
	Check if buffers are the name
	*/
	bool IsSameString(const char* buffer1, const char* buffer2);
	/*
	Check if file path has extension of png or jpg
	*/
	bool IsImage(const char* filePath);
	/*
	Check if file path has extension of fbx
	*/
	bool Is3dScene(const char* filePath);
	/*
	Check status of currently bound framebuffer
	*/
	void CheckFrameBufferStatus();
}
