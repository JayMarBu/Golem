#pragma once
#include "../Device.h"
#include "../FrameInfo.h"
#include "Golem/Temp/TempGameObject.h"
#include "RenderSystemBase.h"

namespace golem
{
	class SimpleRenderSystem : public RenderSystemBase
	{
		// Members ********************************************************************************
	public:
		
	private:
		// Methods ********************************************************************************
	public:
		SimpleRenderSystem(
			Device& _device,
			VkRenderPass _renderPass,
			VkDescriptorSetLayout descriptorSet,
			const std::string& vert_filepath,
			const std::string& frag_filepath
		);
		virtual ~SimpleRenderSystem();

		REMOVE_COPY_CONSTRUCTOR(SimpleRenderSystem);

		void RenderGameObjects(FrameInfo& fInfo, std::vector<TempGameObject>& gameObjects);
	};
}