#version 330 core

out vec4 fragColor;

in vec3 color;
in vec2 texCoords;

uniform sampler2D texUnit;

void main() {
	// fragColor = vec4(color, 1.0);
	// fragColor = texture(texUnit, texCoords) * vec4(color, 1.0);
	// fragColor = texture(texUnit, texCoords) * vec4(color, 1.0);
	fragColor = texture(texUnit, texCoords) * vec4(1.0, 0.0, 0.0, 1.0);

	// fragColor = texture(texUnit, vec2(0.5, 0.5));
}