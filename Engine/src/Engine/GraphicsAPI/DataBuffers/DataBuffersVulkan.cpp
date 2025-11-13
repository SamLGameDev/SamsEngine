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

		vkDestroyBuffer(*SInstance::GetInstance()->GraphicsCard->GetLogicalDevice()->GetVulkanLogicalDevice(), IndexBuffer, nullptr);
		vkFreeMemory(*SInstance::GetInstance()->GraphicsCard->GetLogicalDevice()->GetVulkanLogicalDevice(), IndexMemory, nullptr);
	}

	DataBuffers::DataBuffers()
	{
		
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

	VkBuffer DataBuffers::CreateBuffer(const size_t& Size, const VkBufferUsageFlags& Target,
	                                   const VkMemoryPropertyFlags& Properties, VkDeviceMemory& OutMemory)
	{
		VkBuffer buffer;

		VkBufferCreateInfo bufferInfo{};
		bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		bufferInfo.size = Size;
		bufferInfo.usage = Target;
		bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

		if (vkCreateBuffer(*SInstance::GetInstance()->GraphicsCard->GetLogicalDevice()->GetVulkanLogicalDevice(),
		                   &bufferInfo, nullptr, &buffer) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to Buffer Data");
		};
		VkMemoryRequirements requirements;
		vkGetBufferMemoryRequirements(*SInstance::GetInstance()->GraphicsCard->GetLogicalDevice()->GetVulkanLogicalDevice(), buffer, &requirements);

		VkMemoryAllocateInfo allocateInfo{};
		allocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		allocateInfo.allocationSize = requirements.size;
		allocateInfo.memoryTypeIndex = SInstance::GetInstance()->GraphicsCard->FindMemoryType(requirements.memoryTypeBits,
			Properties);

		if (vkAllocateMemory(*SInstance::GetInstance()->GraphicsCard->GetLogicalDevice()->GetVulkanLogicalDevice(), &allocateInfo, nullptr, &OutMemory) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to create buffer memory");
		}

		vkBindBufferMemory(*SInstance::GetInstance()->GraphicsCard->GetLogicalDevice()->GetVulkanLogicalDevice(),
		                   buffer, OutMemory, 0);

		return buffer;
	}

	void Vulkan::DataBuffers::BufferData(const uint32_t& ID, const size_t& Size, void* Data, const BufferTargets& Target)
	{
		DataBuffer& buffer = RegisteredBuffers.at(ID);
		
		VkDeviceMemory stagingMemory;

		VkBuffer stagingBuffer = CreateBuffer(Size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingMemory);


		void* data;
		vkMapMemory(*SInstance::GetInstance()->GraphicsCard->GetLogicalDevice()->GetVulkanLogicalDevice(), stagingMemory, 0, Size, 0, &data);
		memcpy(data, Data, Size);
		vkUnmapMemory(*SInstance::GetInstance()->GraphicsCard->GetLogicalDevice()->GetVulkanLogicalDevice(), stagingMemory);
		VkDeviceMemory destinationMemory;
		buffer.Buffers.Add(CreateBuffer(ID, TargetToVulkan.at(Transfer) | TargetToVulkan.at(Target), VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, destinationMemory));

		buffer.BufferMemory.Add(destinationMemory);

		CopyBuffer(stagingBuffer,*buffer.Buffers.GetLastPtr(), Size);


		vkDestroyBuffer(*SInstance::GetInstance()->GraphicsCard->GetLogicalDevice()->GetVulkanLogicalDevice(), stagingBuffer, nullptr);
		vkFreeMemory(*SInstance::GetInstance()->GraphicsCard->GetLogicalDevice()->GetVulkanLogicalDevice(), stagingMemory, nullptr);

	}

	void DataBuffers::BufferDataIndex(const uint32_t ID, const size_t& Size, void* Data)
	{
		DataBuffer& buffer = RegisteredBuffers.at(ID);

		VkDeviceMemory stagingMemory;

		VkBuffer stagingBuffer = CreateBuffer(Size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingMemory);


		void* data;
		vkMapMemory(*SInstance::GetInstance()->GraphicsCard->GetLogicalDevice()->GetVulkanLogicalDevice(), stagingMemory, 0, Size, 0, &data);
		memcpy(data, Data, Size);
		vkUnmapMemory(*SInstance::GetInstance()->GraphicsCard->GetLogicalDevice()->GetVulkanLogicalDevice(), stagingMemory);
		VkDeviceMemory destinationMemory;
		buffer.IndexBuffer = CreateBuffer(ID, TargetToVulkan.at(Transfer) | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, destinationMemory);

		buffer.IndexMemory = destinationMemory;

		CopyBuffer(stagingBuffer, buffer.IndexBuffer, Size);


		vkDestroyBuffer(*SInstance::GetInstance()->GraphicsCard->GetLogicalDevice()->GetVulkanLogicalDevice(), stagingBuffer, nullptr);
		vkFreeMemory(*SInstance::GetInstance()->GraphicsCard->GetLogicalDevice()->GetVulkanLogicalDevice(), stagingMemory, nullptr);
	}

	void DataBuffers::CopyBuffer(const VkBuffer& SrcBuffer, VkBuffer& DstBuffer, const VkDeviceSize& Size)
	{

		VkCommandPool* transferPool = SInstance::GetInstance()->GraphicsCard->GetRenderer()->GetTransferPool();

		VkCommandBufferAllocateInfo allocInfo{};
		allocInfo.commandBufferCount = 1;
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocInfo.commandPool = *transferPool;

		VkCommandBuffer cBuffer;
		vkAllocateCommandBuffers(*SInstance::GetInstance()->GraphicsCard->GetLogicalDevice()->GetVulkanLogicalDevice(), &allocInfo, &cBuffer);

		VkCommandBufferBeginInfo beginInfo{};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
		vkBeginCommandBuffer(cBuffer, &beginInfo);

		VkBufferCopy copyBuffer{};
		copyBuffer.srcOffset = 0;
		copyBuffer.dstOffset = 0;
		copyBuffer.size = Size;
		vkCmdCopyBuffer(cBuffer, SrcBuffer, DstBuffer, 1, &copyBuffer);

		vkEndCommandBuffer(cBuffer);

		VkSubmitInfo sInfo{};
		sInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		sInfo.commandBufferCount = 1;
		sInfo.pCommandBuffers = &cBuffer;

		VkFence& copyFence = SInstance::GetInstance()->GraphicsCard->GetRenderer()->GetCopyFence();

		vkResetFences(*SInstance::GetInstance()->GraphicsCard->GetLogicalDevice()->GetVulkanLogicalDevice(), 1, &copyFence);

		vkQueueSubmit(SInstance::GetInstance()->GraphicsCard->GetLogicalDevice()->GetGraphicsQueue(), 1, &sInfo, copyFence);
		vkWaitForFences(*SInstance::GetInstance()->GraphicsCard->GetLogicalDevice()->GetVulkanLogicalDevice(), 1, &copyFence, VK_TRUE, UINT64_MAX);

		vkFreeCommandBuffers(*SInstance::GetInstance()->GraphicsCard->GetLogicalDevice()->GetVulkanLogicalDevice(), *transferPool, 1, &cBuffer);
	}

	void* DataBuffers::GenerateUniformDataBuffer(const uint32_t ID, const size_t& Size)
	{
		DataBuffer& buffer = RegisteredBuffers.at(ID);

		VkDeviceMemory stagingMemory;

		buffer.Buffers.Add(CreateBuffer(Size, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingMemory));
		buffer.BufferMemory.Add(stagingMemory);

		void* data;
		vkMapMemory(*SInstance::GetInstance()->GraphicsCard->GetLogicalDevice()->GetVulkanLogicalDevice(), stagingMemory, 0, Size, 0, &data);

		return data;
	}

	BaseDataBuffer* DataBuffers::GetBuffer(const uint32_t& ID)
	{
		return &RegisteredBuffers.at(ID);
	}

	void Vulkan::DataBuffers::DrawVertexData(const uint32_t& ID)
	{
		const DataBuffer& buffer = RegisteredBuffers.at(ID);

		VkDeviceSize offsets[] = { 0, 0 };
		vkCmdBindVertexBuffers(SInstance::GetInstance()->GraphicsCard->GetRenderer()->GetCurrentBuffer(),
			0, buffer.Buffers.GetSize(), buffer.Buffers.GetFirstRef(), offsets);

		vkCmdBindIndexBuffer(SInstance::GetInstance()->GraphicsCard->GetRenderer()->GetCurrentBuffer(), buffer.IndexBuffer, 0, VK_INDEX_TYPE_UINT16);
	}
}
