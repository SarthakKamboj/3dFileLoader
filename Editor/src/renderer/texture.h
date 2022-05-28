#pragma once

class Texture {
public:
	Texture(const char* filePath, int texUnit);
	void bind();
	void unbind();

private:
	int texUnit;
	unsigned int texture;
};
