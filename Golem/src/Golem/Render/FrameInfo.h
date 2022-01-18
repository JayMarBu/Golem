#pragma once
#include "golpch.h"
#include "Golem/Camera.h"

namespace JEngine
{
	struct GOLEM_API FrameInfo
	{
		int frameIndex;
		float frameTime;
		VkCommandBuffer commandBuffer;
		//Camera& camera;
		VkDescriptorSet globalDescriptorSet;
	};
}