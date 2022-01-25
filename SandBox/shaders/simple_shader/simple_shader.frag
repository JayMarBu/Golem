#version 450

layout (location = 0) in vec3 inColour;
layout (location = 1) in vec2 inTexCoord;
layout (location = 2) in vec3 fragPosWorld;
layout (location = 3) in vec3 fragNormalWorld;

layout (location = 0) out vec4 outColour;

layout (binding = 1) uniform sampler2D texSampler;

struct PointLight
{
	vec4 pos;
	vec4 colour;
};

layout(set = 0, binding = 0) uniform GlobalUbo
{
	mat4 projection;
	mat4 view;
	vec4 ambientColour;
	PointLight pointLights[10];
	int numLights;
} ubo;

layout(push_constant) uniform Push
{
	mat4 modelMatrix;
	mat4 normalMatrix;
} push;

void main()
{
	vec3 diffuseLight = ubo.ambientColour.xyz * ubo.ambientColour.w;
	vec3 surfaceNormal = normalize(fragNormalWorld);

	for(int i = 0; i < ubo.numLights; i ++)
	{
		PointLight pLight = ubo.pointLights[i];

		vec3 directionToLight = pLight.pos.xyz - fragPosWorld;
		float attenuation = 1.0/dot(directionToLight,directionToLight);

		vec3 intensity = pLight.colour.xyz * pLight.colour.w * attenuation;

		diffuseLight += intensity * max(dot(surfaceNormal, normalize(directionToLight)),0);
	}

	outColour = vec4(diffuseLight * inColour, 1.0)*texture(texSampler, inTexCoord);
}