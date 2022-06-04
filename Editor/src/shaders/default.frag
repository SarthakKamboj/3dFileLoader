#version 330 core

out vec4 FragColor;

uniform vec3 color;
uniform int renderTexture;
uniform vec3 viewPos;

struct Light {
	vec3 pos;
	vec3 color;
	float ambientFactor;
};

struct Material {
	sampler2D diffuse;
	float specularStrength;
	float shininess;
};

uniform Material material;
uniform Light light;

in vec2 texCoords;
in vec3 worldPos;
in vec3 normal;

void main() {
	vec4 objectColor = vec4(0,0,0,0);
	if (renderTexture == 1) {
		objectColor = texture(material.diffuse, texCoords);
	} else {
		objectColor = vec4(color, 1.0);
	}

	vec3 normNormal = normalize(normal);
	vec3 normLightDir = normalize(light.pos - worldPos);
	float diffuseFactor = max(dot(normNormal, normLightDir), 0.0);
	vec4 diffuse = vec4(light.color * diffuseFactor, 1);

	vec3 reflectLightDir = reflect(-normLightDir, normNormal);
	vec3 normViewDir = normalize(viewPos - worldPos);
	float specularFactor = pow(max(dot(normViewDir, reflectLightDir), 0.0), material.shininess);
	vec4 specular = vec4(light.color * material.specularStrength * specularFactor, 1);

	vec4 ambient = vec4(light.color * light.ambientFactor, 1);
	FragColor = objectColor * (ambient + diffuse + specular);
}