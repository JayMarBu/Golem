#version 450

layout (location = 0) in vec3 inColour;
layout (location = 1) in vec2 inTexCoord;

layout (location = 0) out vec4 outColour;


layout (binding = 1) uniform sampler2D texSampler;

layout(push_constant) uniform Push
{
	mat4 modelMatrix;
	mat4 normalMatrix;
} push;

void main()
{
	//outColour = vec4(inColour, 1.0);
	outColour = texture(texSampler, inTexCoord);
}