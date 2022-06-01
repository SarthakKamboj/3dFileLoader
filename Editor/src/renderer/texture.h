#pragma once

class Texture {
public:
	Texture();
	Texture(const char* filePath, int texUnit);
	void bind();
	void unbind();
	void updateTextureFilePath(const char* _filePath);

	int texUnit;
	unsigned int texture;
	char filePath[200];
};
