#pragma once
#include "Golem/Render/Device.h"

namespace golem
{

	class Buffer {
		// Members ********************************************************************************
	private:
		Device& m_device;
		void* m_mapped = nullptr;
		VkBuffer m_buffer = VK_NULL_HANDLE;
		VkDeviceMemory m_memory = VK_NULL_HANDLE;

		VkDeviceSize m_bufferSize;
		uint32_t m_instanceCount;
		VkDeviceSize m_instanceSize;
		VkDeviceSize m_alignmentSize;
		VkBufferUsageFlags m_usageFlags;
		VkMemoryPropertyFlags m_memoryPropertyFlags;

		// Methods ********************************************************************************
	public:
		Buffer(
			Device& device,
			VkDeviceSize instanceSize,
			uint32_t instanceCount,
			VkBufferUsageFlags usageFlags,
			VkMemoryPropertyFlags memoryPropertyFlags,
			VkDeviceSize minOffsetAlignment = 1);
		~Buffer();

		Buffer(const Buffer&) = delete;
		Buffer& operator=(const Buffer&) = delete;

		VkResult Map(VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0);
		void Unmap();

		void WriteToBuffer(void* data, VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0);
		VkResult Flush(VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0);
		inline VkDescriptorBufferInfo DescriptorInfo(VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0) const { return VkDescriptorBufferInfo{ m_buffer,offset,size, }; }
		VkResult Invalidate(VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0);

		inline void WriteToIndex(void* data, int index) { WriteToBuffer(data, m_instanceSize, index * m_alignmentSize); }
		inline VkResult FlushIndex(int index) { return Flush(m_alignmentSize, index * m_alignmentSize); }
		inline VkDescriptorBufferInfo DescriptorInfoForIndex(int index) { return DescriptorInfo(m_alignmentSize, index * m_alignmentSize); }
		inline VkResult InvalidateIndex(int index) { return Invalidate(m_alignmentSize, index * m_alignmentSize); }

		inline VkBuffer GetBuffer() const { return m_buffer; }
		inline void* GetMappedMemory() const { return m_mapped; }
		inline uint32_t GetInstanceCount() const { return m_instanceCount; }
		inline VkDeviceSize GetInstanceSize() const { return m_instanceSize; }
		inline VkDeviceSize GetAlignmentSize() const { return m_instanceSize; }
		inline VkBufferUsageFlags GetUsageFlags() const { return m_usageFlags; }
		inline VkMemoryPropertyFlags GetMemoryPropertyFlags() const { return m_memoryPropertyFlags; }
		inline VkDeviceSize GetBufferSize() const { return m_bufferSize; }

	private:
		static VkDeviceSize getAlignment(VkDeviceSize instanceSize, VkDeviceSize minOffsetAlignment);
	};

}