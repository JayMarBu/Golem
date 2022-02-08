#version 450
layout (location = 0) in vec2 UV;
layout (location = 0) out vec4 outColour;

layout (binding = 0) uniform sampler2D samplerColor;

void main() 
{
	outColour = texture(samplerColor, UV);
}
