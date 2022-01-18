#pragma once
#include "golpch.h"
#include "Golem/Camera.h"

namespace JEngine
{
	struct FrameInfo
	{
		int frameIndex;
		float frameTime;
		VkCommandBuffer commandBuffer;
		//Camera& camera;
		VkDescriptorSet globalDescriptorSet;
	};
}