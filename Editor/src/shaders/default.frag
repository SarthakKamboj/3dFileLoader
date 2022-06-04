#version 330 core

out vec4 FragColor;

uniform vec3 color;
uniform int renderTexture;
uniform sampler2D texUnit;

uniform vec3 ambientColor;
uniform float ambientFactor;
uniform vec3 lightPos;
uniform vec3 diffuseColor;
uniform vec3 specularColor;
uniform vec3 viewPos;
uniform float specularStrength;

in vec2 texCoords;
in vec3 worldPos;
in vec3 normal;

void main() {
	if (renderTexture == 1) {
		FragColor = texture(texUnit, texCoords) * vec4(color, 1.0);
	} else {
		FragColor = vec4(color, 1.0);
	}

	vec3 normNormal = normalize(normal);
	vec3 normLightDir = normalize(lightPos - worldPos);
	float diffuseFactor = max(dot(normNormal, normLightDir), 0.0);
	vec4 diffuse = vec4(diffuseColor * diffuseFactor, 1);

	vec3 reflectLightDir = reflect(-normLightDir, normNormal);
	vec3 normViewDir = normalize(viewPos - worldPos);
	float specularFactor = pow(max(dot(normViewDir, reflectLightDir), 0.0), 32);
	vec4 specular = vec4(specularColor * specularStrength * specularFactor, 1);

	vec4 ambient = vec4(ambientColor * ambientFactor, 1);
	FragColor = FragColor * (ambient + diffuse + specular);
}