#version 330 core

out vec4 fragColor;

in vec3 color;
in vec2 texCoords;

uniform sampler2D texUnit;

void main() {
	// fragColor = vec4(0.0, 0.0, 1.0, 1.0) * (gl_FragCoord.y / 800.0);
	// fragColor = vec4(gl_FragCoord.xy / 800.0, 0.0, 1.0);
	// fragColor = vec4(color, 1.0);
	// fragColor = texture(texUnit, texCoords) * vec4(color, 1.0);
	// fragColor = texture(texUnit, texCoords) * vec4(1.0, 0.0, 0.0, 1.0);

	// fragColor = vec4(1.0, 1.0, 1.0, 1.0);
	// fragColor = texture(texUnit, vec2(0.5, 0.5));
	fragColor = texture(texUnit, texCoords);
}