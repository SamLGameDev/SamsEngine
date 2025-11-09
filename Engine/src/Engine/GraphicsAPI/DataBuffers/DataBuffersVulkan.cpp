#include "DataBuffersVulkan.h"
#include<glad/glad.h>

#include "MathCore.h"
#include "GLFW/glfw3.h"
#include <limits>

#include "VulkanInstance.h"
#include "VulkanLogicalDevice.h"
#include <stdexcept>

namespace Vulkan
{



	Vulkan::DataBuffer::~DataBuffer()
	{
		for (auto& buffer : Buffers)
		{
			vkDestroyBuffer(*SInstance::GetInstance()->GraphicsCard->GetLogicalDevice()->GetVulkanLogicalDevice(), buffer, nullptr);
		}

		for (auto& memory : BufferMemory)
		{
			vkFreeMemory(*SInstance::GetInstance()->GraphicsCard->GetLogicalDevice()->GetVulkanLogicalDevice(), memory, nullptr);
		}
	}

	void Vulkan::DataBuffers::GenBuffer(const size_t& Number, Array<uint32_t>& IDs)
	{
		for (size_t i = 0; i < Number; i++)
		{
			uint32_t id;
			do
			{
				id = MathCore::RandomRange<uint32_t>(0, std::numeric_limits<uint32_t>::max());
			} while (RegisteredBuffers.contains(id));
			RegisteredBuffers.insert({ id, DataBuffer() });
			IDs.Add(id);
		}
	}

	void DataBuffers::GenBuffer(uint32_t& ID)
	{
		do
		{
			ID = MathCore::RandomRange<uint32_t>(0, std::numeric_limits<uint32_t>::max());
		} while (RegisteredBuffers.contains(ID));
		RegisteredBuffers.insert({ ID, DataBuffer() });
		
	}

	void Vulkan::DataBuffers::BindVertexInfo(const uint32_t& ID, const size_t& Location, const size_t& Size,
	                                         const size_t& Stride, const size_t& Offset)
	{
		DataBuffer& buffer = RegisteredBuffers.at(ID);

		VkVertexInputBindingDescription2EXT bindingDescription{};
		bindingDescription.sType = VK_STRUCTURE_TYPE_VERTEX_INPUT_BINDING_DESCRIPTION_2_EXT;
		bindingDescription.binding = Location;
		bindingDescription.stride = Stride;
		bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
		bindingDescription.divisor = 1;
		bindingDescription.pNext = nullptr;
		buffer.BindingDescriptions.Add(bindingDescription);

		VkVertexInputAttributeDescription2EXT attributeDescription{};
		attributeDescription.sType = VK_STRUCTURE_TYPE_VERTEX_INPUT_ATTRIBUTE_DESCRIPTION_2_EXT;
		attributeDescription.binding = Location;
		attributeDescription.location = Location;
		attributeDescription.offset = Offset;
		attributeDescription.format = Location == 1 ? VK_FORMAT_R32G32B32_SFLOAT : VK_FORMAT_R32G32_SFLOAT;
		attributeDescription.pNext = nullptr;
		buffer.AttributeDescriptions.Add(attributeDescription);
	}

	void Vulkan::DataBuffers::BindBuffer(const uint32_t& ID)
	{
		DataBuffer& buffer = RegisteredBuffers.at(ID);

		vkCmdSetVertexInputEXT(
			SInstance::GetInstance()->GraphicsCard->GetRenderer()->GetCurrentBuffer(),
			buffer.BindingDescriptions.GetSize(),
			buffer.BindingDescriptions.GetFirstRef(),
			buffer.AttributeDescriptions.GetSize(),
			buffer.AttributeDescriptions.GetFirstRef()
		);
	}

	void Vulkan::DataBuffers::BufferData(const uint32_t& ID, const size_t& Size, void* Data, const BufferTargets& Target)
	{
		VkBufferCreateInfo bufferInfo{};
		bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		bufferInfo.size = Size;
		bufferInfo.usage = TargetToVulkan.at(Target);
		bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

		DataBuffer& buffer = RegisteredBuffers.at(ID);
		buffer.Buffers.Reallocate(buffer.Buffers.GetSize() + 1);

		if (vkCreateBuffer(*SInstance::GetInstance()->GraphicsCard->GetLogicalDevice()->GetVulkanLogicalDevice(),
			&bufferInfo, nullptr, buffer.Buffers.GetLastPtr()) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to Buffer Data");
		};
		VkMemoryRequirements requirements;
		vkGetBufferMemoryRequirements(*SInstance::GetInstance()->GraphicsCard->GetLogicalDevice()->GetVulkanLogicalDevice(), *buffer.Buffers.GetLastPtr(), &requirements);

		VkMemoryAllocateInfo allocateInfo{};
		allocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		allocateInfo.allocationSize = requirements.size;
		allocateInfo.memoryTypeIndex = SInstance::GetInstance()->GraphicsCard->FindMemoryType(requirements.memoryTypeBits,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

		buffer.BufferMemory.Reallocate(buffer.BufferMemory.GetSize() + 1);

		if (vkAllocateMemory(*SInstance::GetInstance()->GraphicsCard->GetLogicalDevice()->GetVulkanLogicalDevice(), &allocateInfo, nullptr, buffer.BufferMemory.GetLastPtr()) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to create buffer memory");
		}

		vkBindBufferMemory(*SInstance::GetInstance()->GraphicsCard->GetLogicalDevice()->GetVulkanLogicalDevice(),
			*buffer.Buffers.GetLastPtr(), *buffer.BufferMemory.GetLastPtr(), 0);

		void* data;
		vkMapMemory(*SInstance::GetInstance()->GraphicsCard->GetLogicalDevice()->GetVulkanLogicalDevice(), *buffer.BufferMemory.GetLastPtr(), 0, bufferInfo.size, 0, &data);
		memcpy(data, Data, bufferInfo.size);
		vkUnmapMemory(*SInstance::GetInstance()->GraphicsCard->GetLogicalDevice()->GetVulkanLogicalDevice(), *buffer.BufferMemory.GetLastPtr());

	}

	void Vulkan::DataBuffers::DrawVertexData(const uint32_t& ID)
	{
		const DataBuffer& buffer = RegisteredBuffers.at(ID);

		VkDeviceSize offsets[] = { 0, 0 };
		vkCmdBindVertexBuffers(SInstance::GetInstance()->GraphicsCard->GetRenderer()->GetCurrentBuffer(),
			0, buffer.Buffers.GetSize(), buffer.Buffers.GetFirstRef(), offsets);
	}
}
