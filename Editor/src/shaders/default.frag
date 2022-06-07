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
in vec4 worldPos;
in vec3 normal;

uniform sampler2D depthTexUnit;
// uniform float windowWidth;
// uniform float windowHeight;
uniform float nearPlane;
uniform float farPlane;

uniform mat4 lightView;
uniform mat4 projection;

float linearize(float depth) {
	return (nearPlane * farPlane) / (farPlane - (depth * (farPlane - nearPlane)));
}

float ndcToZeroToOne(float ndc) {
	return (ndc * 0.5) + 0.5;
}

void main() {
	vec4 objectColor = vec4(0,0,0,0);
	if (renderTexture == 1) {
		objectColor = texture(material.diffuse, texCoords);
	} else {
		objectColor = vec4(color, 1.0);
	}

	vec3 normNormal = normalize(normal);
	vec3 normLightDir = normalize(light.pos - worldPos.xyz);
	float diffuseFactor = max(dot(normNormal, normLightDir), 0.0);
	vec4 diffuse = vec4(light.color * diffuseFactor, 1);

	vec3 reflectLightDir = reflect(-normLightDir, normNormal);
	vec3 normViewDir = normalize(viewPos - worldPos.xyz);
	float specularFactor = pow(max(dot(normViewDir, reflectLightDir), 0.0), material.shininess);
	vec4 specular = vec4(light.color * material.specularStrength * specularFactor, 1);

	vec4 ambient = vec4(light.color * light.ambientFactor, 1);

	vec4 curPosRelToLight = projection * lightView * worldPos;
	curPosRelToLight = curPosRelToLight / curPosRelToLight.w;
	float curLightDist = -(1 - ndcToZeroToOne(curPosRelToLight.z));
	
	vec2 tex = vec2(ndcToZeroToOne(curPosRelToLight.x), ndcToZeroToOne(curPosRelToLight.y));
	// float closestToLightDist = linearize(texture(depthTexUnit, tex).x);
	float closestToLightDist = texture(depthTexUnit, tex).x;
	// if (curLightDist <= closestToLightDist) {
	if (0.99 >= closestToLightDist) {
		FragColor = objectColor * (ambient + diffuse + specular);
	} else {
		FragColor = vec4(1,0,0,1);
	}

	// FragColor = vec4(tex.x, 0, 0,1);
	// FragColor = vec4(0, tex.y, 0,1);

}