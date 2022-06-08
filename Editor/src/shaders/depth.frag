#version 330 core

uniform sampler2D depthTexUnit;
uniform bool extraVisible;

in vec2 tex;

out vec4 FragColor;

void main() {
	float val;
	if (extraVisible) {
		val = pow(texture(depthTexUnit, tex).x, 32);
	} else {
		val = texture(depthTexUnit, tex).x;
	}
	FragColor = vec4(val, val, val, 1.0);
}