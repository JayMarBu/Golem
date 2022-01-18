#include "golpch.h"
#include "Golem/Render/Buffer.h"

namespace golem
{

	VkDeviceSize Buffer::getAlignment(VkDeviceSize instanceSize, VkDeviceSize minOffsetAlignment)
	{
		if (minOffsetAlignment > 0)
		{
			return (instanceSize + minOffsetAlignment - 1) & ~(minOffsetAlignment - 1);
		}
		return instanceSize;
	}

	Buffer::Buffer(Device& device, VkDeviceSize instanceSize, uint32_t instanceCount, VkBufferUsageFlags usageFlags, VkMemoryPropertyFlags memoryPropertyFlags, VkDeviceSize minOffsetAlignment /*= 1*/)
		: m_device{ device },
		m_instanceSize{ instanceSize },
		m_instanceCount{ instanceCount },
		m_usageFlags{ usageFlags },
		m_memoryPropertyFlags{ memoryPropertyFlags }
	{
		m_alignmentSize = getAlignment(instanceSize, minOffsetAlignment);
		m_bufferSize = m_alignmentSize * instanceCount;
		device.CreateBuffer(m_bufferSize, usageFlags, memoryPropertyFlags, m_buffer, m_memory);
	}

	VkResult Buffer::Map(VkDeviceSize size /*= VK_WHOLE_SIZE*/, VkDeviceSize offset /*= 0*/)
	{
		assert(m_buffer && m_memory && "Called map on buffer before create");
		return vkMapMemory(m_device.device(), m_memory, offset, size, 0, &m_mapped);
	}

	void Buffer::Unmap()
	{
		if (m_mapped)
		{
			vkUnmapMemory(m_device.device(), m_memory);
			m_mapped = nullptr;
		}
	}

	void Buffer::WriteToBuffer(void* data, VkDeviceSize size /*= VK_WHOLE_SIZE*/, VkDeviceSize offset /*= 0*/)
	{
		assert(m_mapped && "Cannot copy to unmapped buffer");

		if (size == VK_WHOLE_SIZE)
		{
			memcpy(m_mapped, data, m_bufferSize);
		}
		else
		{
			char* memOffset = (char*)m_mapped;
			memOffset += offset;
			memcpy(memOffset, data, size);
		}
	}

	VkResult Buffer::Flush(VkDeviceSize size /*= VK_WHOLE_SIZE*/, VkDeviceSize offset /*= 0*/)
	{
		VkMappedMemoryRange mappedRange = {};
		mappedRange.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
		mappedRange.memory = m_memory;
		mappedRange.offset = offset;
		mappedRange.size = size;
		return vkFlushMappedMemoryRanges(m_device.device(), 1, &mappedRange);
	}

	VkResult Buffer::Invalidate(VkDeviceSize size /*= VK_WHOLE_SIZE*/, VkDeviceSize offset /*= 0*/)
	{
		VkMappedMemoryRange mappedRange = {};
		mappedRange.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
		mappedRange.memory = m_memory;
		mappedRange.offset = offset;
		mappedRange.size = size;
		return vkInvalidateMappedMemoryRanges(m_device.device(), 1, &mappedRange);
	}

	Buffer::~Buffer()
	{
		Unmap();
		vkDestroyBuffer(m_device.device(), m_buffer, nullptr);
		vkFreeMemory(m_device.device(), m_memory, nullptr);
	}

}