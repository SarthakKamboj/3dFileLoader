#include "helper.h"

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
}