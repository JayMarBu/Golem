#pragma once
#include "Golem/Render/Device.h"

namespace golem
{
	// ******************************************************************************************//
	// Descriptor set layout class																 //
	// ******************************************************************************************//
	class DescriptorSetLayout
	{
		// Members ********************************************************************************
	public:

		class Builder
		{
		private:
			Device& m_device;
			std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> m_bindings{};

		public:
			Builder(Device& device) : m_device{ device } {}

			Builder& AddBinding(
				uint32_t binding,
				VkDescriptorType descriptorType,
				VkShaderStageFlags stageFlags,
				uint32_t count = 1);
			Scope<DescriptorSetLayout> Build() const;

		};

	private:
		Device& m_device;
		VkDescriptorSetLayout m_descriptorSetLayout;
		std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> m_bindings;

		friend class DescriptorWriter;

		// Methods ********************************************************************************
	public:

		DescriptorSetLayout(Device& device, std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> bindings);
		~DescriptorSetLayout();

		REMOVE_COPY_CONSTRUCTOR(DescriptorSetLayout);

		VkDescriptorSetLayout GetDescriptorSetLayout() const { return m_descriptorSetLayout; }
	};

	// ******************************************************************************************//
	// Descriptor pool class																	 //
	// ******************************************************************************************//
	class DescriptorPool
	{
		// Members ********************************************************************************
	public:

		class Builder
		{
		private:
			Device& m_device;
			std::vector<VkDescriptorPoolSize> m_poolSizes{};
			uint32_t m_maxSets = 1000;
			VkDescriptorPoolCreateFlags m_poolFlags = 0;

		public:
			Builder(Device& device) : m_device{ device } {}

			Builder& AddPoolSize(VkDescriptorType descriptorType, uint32_t count);
			Builder& SetPoolFlags(VkDescriptorPoolCreateFlags flags);
			Builder& SetMaxSets(uint32_t count);
			Scope<DescriptorPool> Build() const;
		};

	private:
		Device& m_device;
		VkDescriptorPool m_descriptorPool;

		friend class DescriptorWriter;

		// Methods ********************************************************************************
	public:
		DescriptorPool(
			Device& device,
			uint32_t maxSets,
			VkDescriptorPoolCreateFlags poolFlags,
			const std::vector<VkDescriptorPoolSize>& poolSizes);
		~DescriptorPool();

		REMOVE_COPY_CONSTRUCTOR(DescriptorPool);

		inline VkDescriptorPool GetVKDescriptorPool() {return m_descriptorPool;}

		bool AllocateDescriptor(const VkDescriptorSetLayout descriptorSetLayout, VkDescriptorSet& descriptor) const;

		void FreeDescriptors(std::vector<VkDescriptorSet>& descriptors) const;

		void ResetPool();
	};

	// ******************************************************************************************//
	// Descriptor writer class																	 //
	// ******************************************************************************************//
	class DescriptorWriter
	{
		// Members ********************************************************************************
	private:
		DescriptorSetLayout& m_setLayout;
		DescriptorPool& m_pool;
		std::vector<VkWriteDescriptorSet> m_writes;

		// Methods ********************************************************************************
	public:
		DescriptorWriter(DescriptorSetLayout& setLayout, DescriptorPool& pool);

		DescriptorWriter& WriteBuffer(uint32_t binding, VkDescriptorBufferInfo* bufferInfo);
		DescriptorWriter& WriteImage(uint32_t binding, VkDescriptorImageInfo* imageInfo);

		bool Build(VkDescriptorSet& set);
		void Overwrite(VkDescriptorSet& set);

	};
}