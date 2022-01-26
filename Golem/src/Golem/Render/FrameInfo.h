#pragma once
#include "golpch.h"
#include "Golem/Render/Camera.h"

namespace golem
{
	#define MAX_LIGHTS 10

	struct PointLight
	{
		glm::vec3 pos = glm::normalize(glm::vec3(1, -3, -1));
		alignas(16) glm::vec4 lightColour{ 1.0f,1.0f,1.0f,10.0f };
	};

	struct GlobalUBO
	{
		glm::mat4 projection{ 1.0f };
		glm::mat4 view{ 1.0f };
		glm::vec4 ambientLightColour{ 1.0f, 1.0f,1.0f,.02f };
		PointLight pointLights[MAX_LIGHTS];
		int numLights;
	};

	struct FrameInfo
	{
		int frameIndex;
		float frameTime;
		VkCommandBuffer commandBuffer;
		Camera& camera;
		glm::vec3 camPos;
		VkDescriptorSet globalDescriptorSet;
	};
}