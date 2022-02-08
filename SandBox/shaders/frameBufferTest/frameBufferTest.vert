#version 450
#extension GL_KHR_vulkan_glsl : enable

layout (location = 0) out vec2 outUV;

vec2 positions[6] = vec2[]
(
	vec2(0.25, 0.95),	// BL
	vec2(0.25, 0.25),	// TL
	vec2(0.95, 0.95),	// BR

	vec2(0.95, 0.95),	// BR
	vec2(0.25, 0.25),	// TL
	vec2(0.95, 0.25)	// TR
);

vec2 uvs[6] = vec2[]
(
	vec2(0, 1),
	vec2(0, 0),
	vec2(1, 1),
	vec2(1, 1),
	vec2(0, 0),
	vec2(1, 0)
);

void main()
{
	gl_Position = vec4(positions[gl_VertexIndex], 0.0, 1.0);
	outUV = uvs[gl_VertexIndex];
}