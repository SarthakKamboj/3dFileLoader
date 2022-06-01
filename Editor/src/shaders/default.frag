#version 330 core

out vec4 FragColor;

uniform vec3 color;
uniform int renderTexture;
uniform sampler2D texUnit;

in vec2 texCoords;

void main() {
	if (renderTexture == 1) {
		FragColor = texture(texUnit, texCoords) * vec4(color, 1.0);
	} else {
		FragColor = vec4(color, 1.0);
	}
}