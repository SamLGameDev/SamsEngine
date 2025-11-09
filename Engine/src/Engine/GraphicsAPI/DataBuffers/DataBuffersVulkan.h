
#pragma once
#include "BaseDataBuffers.h"
#include "ErrorCodes.h"
#include "InitialiseVulkan.h"
#include <map>

#include "VulkanInstance.h"
#include "VulkanLogicalDevice.h"
#include "glm/fwd.hpp"


inline void vkCmdSetVertexInputEXT(
	VkCommandBuffer buffer, const uint32_t bindingCount, const VkVertexInputBindingDescription2EXT* bindings, uint32_t attributeCount, const VkVertexInputAttributeDescription2EXT* attributes)
{
	static PFN_vkCmdSetVertexInputEXT func = nullptr;
	if (!func)
	{
		func = (PFN_vkCmdSetVertexInputEXT)vkGetDeviceProcAddr(*Vulkan::SInstance::GetInstance()->GraphicsCard->GetLogicalDevice()->GetVulkanLogicalDevice(), "vkCmdSetVertexInputEXT");
	}

	if (func == VK_NULL_HANDLE)
	{
		return;
	}
	func(buffer, bindingCount, bindings, attributeCount, attributes);
}
namespace Vulkan
{



	class DataBuffer : public BaseDataBuffer
	{
	public:

		~DataBuffer();

		DataBuffer() = default;

		Array<VkVertexInputBindingDescription2EXT> BindingDescriptions;
		Array<VkVertexInputAttributeDescription2EXT> AttributeDescriptions;

		Array<VkBuffer> Buffers;
		Array<VkDeviceMemory> BufferMemory;
	};

	class DataBuffers : public BaseDataBuffers
	{
	public:

		DataBuffers() = default;

		void GenBuffer(const size_t& Number, Array<uint32_t>& IDs) override;
		void GenBuffer(uint32_t& ID) override;

		void BindVertexInfo(const uint32_t& ID, const size_t& Location, const size_t& Size, const size_t& Stride, const size_t& Offset) override;

		void BindBuffer(const uint32_t& ID) override;
		void BufferData(const uint32_t& ID, const size_t& Size, void* Data, const BufferTargets& Target) override;

		void DrawVertexData(const uint32_t& ID) override;

		std::map<uint32_t, DataBuffer> RegisteredBuffers;

		std::map<BufferTargets, VkBufferUsageFlags> TargetToVulkan{ {BufferTargets::Vertex, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT}, {Indices, VK_BUFFER_USAGE_INDEX_BUFFER_BIT} };
	};

}
