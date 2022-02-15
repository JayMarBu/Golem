#include "golpch.h"
#include "Golem/Render/Descriptors.h"
#include "Golem/Core.h"

namespace golem
{
	// ******************************************************************************************//
	// Descriptor set layout builder methods													 //
	// ******************************************************************************************//

	DescriptorSetLayout::Builder& DescriptorSetLayout::Builder::AddBinding(uint32_t binding, VkDescriptorType descriptorType, VkShaderStageFlags stageFlags, uint32_t count /*= 1*/)
	{
		assert(m_bindings.count(binding) == 0 && "Binding already in use");
		VkDescriptorSetLayoutBinding layoutBinding{};
		layoutBinding.binding = binding;
		layoutBinding.descriptorType = descriptorType;
		layoutBinding.descriptorCount = count;
		layoutBinding.stageFlags = stageFlags;
		layoutBinding.pImmutableSamplers = nullptr;
		m_bindings[binding] = layoutBinding;
		return *this;
	}

	Scope<DescriptorSetLayout> DescriptorSetLayout::Builder::Build() const
	{
		return CreateScope<DescriptorSetLayout>(m_device, m_bindings);
	}

	// ******************************************************************************************//
	// Descriptor set layout class methods														 //
	// ******************************************************************************************//

	DescriptorSetLayout::DescriptorSetLayout(Device& device, std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> bindings)
		: m_device{ device }, m_bindings{ bindings }
	{
		std::vector<VkDescriptorSetLayoutBinding> setLayoutBindings{};
		for (auto kv : bindings)
		{
			setLayoutBindings.push_back(kv.second);
		}

		VkDescriptorSetLayoutCreateInfo descriptorSetLayoutInfo{};
		descriptorSetLayoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		descriptorSetLayoutInfo.bindingCount = static_cast<uint32_t>(setLayoutBindings.size());
		descriptorSetLayoutInfo.pBindings = setLayoutBindings.data();

		SAFE_RUN_VULKAN_FUNC(vkCreateDescriptorSetLayout(m_device.device(), &descriptorSetLayoutInfo, nullptr, &m_descriptorSetLayout),
			"failed to create descriptor set layout!");
	}

	DescriptorSetLayout::~DescriptorSetLayout()
	{
		vkDestroyDescriptorSetLayout(m_device, m_descriptorSetLayout, nullptr);
	}

	// ******************************************************************************************//
	// Descriptor pool builder methods															 //
	// ******************************************************************************************//

	DescriptorPool::Builder& DescriptorPool::Builder::AddPoolSize(VkDescriptorType descriptorType, uint32_t count)
	{
		m_poolSizes.push_back({ descriptorType, count });
		return *this;
	}

	DescriptorPool::Builder& DescriptorPool::Builder::SetPoolFlags(VkDescriptorPoolCreateFlags flags)
	{
		m_poolFlags = flags;
		return *this;
	}

	DescriptorPool::Builder& DescriptorPool::Builder::SetMaxSets(uint32_t count)
	{
		m_maxSets = count;
		return *this;
	}

	Scope<DescriptorPool> DescriptorPool::Builder::Build() const
	{
		return CreateScope<DescriptorPool>(m_device, m_maxSets, m_poolFlags, m_poolSizes);
	}

	// ******************************************************************************************//
	// Descriptor pool class methods															 //
	// ******************************************************************************************//

	DescriptorPool::DescriptorPool(Device& device, uint32_t maxSets, VkDescriptorPoolCreateFlags poolFlags, const std::vector<VkDescriptorPoolSize>& poolSizes)
		: m_device{ device }
	{
		VkDescriptorPoolCreateInfo descriptorPoolInfo{};
		descriptorPoolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		descriptorPoolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
		descriptorPoolInfo.pPoolSizes = poolSizes.data();
		descriptorPoolInfo.maxSets = maxSets;
		descriptorPoolInfo.flags = poolFlags;

		SAFE_RUN_VULKAN_FUNC(vkCreateDescriptorPool(m_device.device(), &descriptorPoolInfo, nullptr, &m_descriptorPool), "failed to create descriptor pool!")
	}

	bool DescriptorPool::AllocateDescriptor(const VkDescriptorSetLayout descriptorSetLayout, VkDescriptorSet& descriptor) const
	{
		VkDescriptorSetAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
		allocInfo.descriptorPool = m_descriptorPool;
		allocInfo.pSetLayouts = &descriptorSetLayout;
		allocInfo.descriptorSetCount = 1;

		// Might want to create a "DescriptorPoolManager" class that handles this case, and builds
		// a new pool whenever an old pool fills up. But this is beyond our current scope
		if (vkAllocateDescriptorSets(m_device.device(), &allocInfo, &descriptor) != VK_SUCCESS)
		{
			return false;
		}
		return true;
	}

	void DescriptorPool::FreeDescriptors(std::vector<VkDescriptorSet>& descriptors) const
	{
		vkFreeDescriptorSets(
			m_device.device(),
			m_descriptorPool,
			static_cast<uint32_t>(descriptors.size()),
			descriptors.data());
	}

	void DescriptorPool::ResetPool()
	{
		vkResetDescriptorPool(m_device.device(), m_descriptorPool, 0);
	}

	DescriptorPool::~DescriptorPool()
	{
		vkDestroyDescriptorPool(m_device.device(), m_descriptorPool, nullptr);
	}

	// ******************************************************************************************//
	// Descriptor writer class	methods															 //
	// ******************************************************************************************//

	DescriptorWriter::DescriptorWriter(DescriptorSetLayout& setLayout, DescriptorPool& pool)
		: m_setLayout{ setLayout }, m_pool{ pool }
	{

	}

	DescriptorWriter& DescriptorWriter::WriteBuffer(uint32_t binding, VkDescriptorBufferInfo* bufferInfo)
	{
		assert(m_setLayout.m_bindings.count(binding) == 1 && "Layout does not contain specified binding");

		auto& bindingDescription = m_setLayout.m_bindings[binding];

		assert(bindingDescription.descriptorCount == 1 && "Binding single descriptor info, but binding expects multiple");

		VkWriteDescriptorSet write{};
		write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		write.descriptorType = bindingDescription.descriptorType;
		write.dstBinding = binding;
		write.pBufferInfo = bufferInfo;
		write.descriptorCount = 1;

		m_writes.push_back(write);
		return *this;
	}

	DescriptorWriter& DescriptorWriter::WriteImage(uint32_t binding, VkDescriptorImageInfo* imageInfo)
	{
		assert(m_setLayout.m_bindings.count(binding) == 1 && "Layout does not contain specified binding");

		auto& bindingDescription = m_setLayout.m_bindings[binding];

		assert(
			bindingDescription.descriptorCount == 1 &&
			"Binding single descriptor info, but binding expects multiple");

		VkWriteDescriptorSet write{};
		write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		write.descriptorType = bindingDescription.descriptorType;
		write.dstBinding = binding;
		write.pImageInfo = imageInfo;
		write.descriptorCount = 1;

		m_writes.push_back(write);
		return *this;
	}

	bool DescriptorWriter::Build(VkDescriptorSet& set)
	{
		bool success = m_pool.AllocateDescriptor(m_setLayout.GetDescriptorSetLayout(), set);
		if (!success)
			return false;
		Overwrite(set);
		return true;
	}

	void DescriptorWriter::Overwrite(VkDescriptorSet& set)
	{
		for (auto& write : m_writes)
		{
			write.dstSet = set;
		}
		vkUpdateDescriptorSets(m_pool.m_device, m_writes.size(), m_writes.data(), 0, nullptr);
	}

}