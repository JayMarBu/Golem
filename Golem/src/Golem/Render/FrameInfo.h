#pragma once
#include "golpch.h"
#include "Golem/Render/Camera.h"

namespace golem
{
	struct FrameInfo
	{
		int frameIndex;
		float frameTime;
		VkCommandBuffer commandBuffer;
		Camera& camera;
		VkDescriptorSet globalDescriptorSet;
	};
}