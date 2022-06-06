#version 330 core

uniform sampler2D depthTexUnit;

in vec2 tex;

out vec4 FragColor;

void main() {
	float val = pow(texture(depthTexUnit, tex).x, 32);
	FragColor = vec4(val, val, val, 1.0);
}