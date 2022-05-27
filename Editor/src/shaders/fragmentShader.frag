#version 330 core

out vec4 fragColor;

in vec3 color;

float rand(vec2 co){
    return fract(sin(dot(co, vec2(12.9898, 78.233))) * 43758.5453);
}

void main() {
	fragColor = vec4(color, 1.0);
}