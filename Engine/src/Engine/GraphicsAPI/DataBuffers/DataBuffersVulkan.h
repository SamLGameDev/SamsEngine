
#pragma once
#include "BaseDataBuffers.h"
#include "ErrorCodes.h"
#include "InitialiseVulkan.h"
#include <map>

#include "VulkanInstance.h"
#include "VulkanLogicalDevice.h"
#include "glm/fwd.hpp"


class UComputeShader;

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

		~DataBuffer() override;

		DataBuffer() = default;

		Array<VkVertexInputBindingDescription2EXT> BindingDescriptions;
		Array<VkVertexInputAttributeDescription2EXT> AttributeDescriptions;

		Array<VkBuffer> Buffers;
		Array<VkDeviceMemory> BufferMemory;

		VkBuffer IndexBuffer = VK_NULL_HANDLE;
		VkDeviceMemory IndexMemory=  VK_NULL_HANDLE;
	};

	class TextureBuffer : public BaseDataBuffer
	{
	public:

		~TextureBuffer() override;

		TextureBuffer() = default;

		Array<VkImage> Images;
		Array<VkDeviceMemory> BufferMemory;
		Array<VkImageView> ImageViews;
	};

	class DataBuffers : public BaseDataBuffers
	{
	public:

		DataBuffers();

		void GenBuffer(const size_t& Number, Array<uint32_t>& IDs) override;
		void GenBuffer(uint32_t& ID) override;

		void GenTexture(const size_t& Number, Array<uint32_t>& IDs) override;
		void GenTexture(uint32_t& ID) override;

		void BindVertexInfo(const uint32_t& ID, const size_t& Location, const size_t& Size, const size_t& Stride, const size_t& Offset, const
		                    BufferFormat& Format) override;

		void BindBuffer(const uint32_t& ID) override;
		VkBuffer CreateBuffer(const size_t& Size, const VkBufferUsageFlags& Target,
		                      const VkMemoryPropertyFlags& Properties, VkDeviceMemory& OutMemory);
		void BufferData(const uint32_t& ID, const size_t& Size, void* Data, const BufferTargets& Target) override;

		void BufferDataIndex(const uint32_t ID, const size_t& Size, void* Data) override;
		VkCommandPool* BeginSingleTimeCommands(VkCommandBuffer& CBuffer);
		void EndSingleTimeCommands(VkCommandBuffer CBuffer, VkCommandPool* TransferPool);

		void CopyBuffer(const VkBuffer& SrcBuffer, VkBuffer& DstBuffer, const VkDeviceSize& Size);

		void* GenerateUniformDataBuffer(const uint32_t ID, const size_t& Size) override;

		void GenerateShaderStorageBuffer(const uint32_t ID, const size_t& Size, const size_t& Binding) override;

		void BindShaderStorageBuffer(uint32_t ID, const size_t& Binding, const size_t& Size) override;

		void* MapBufferMemory(const uint32_t& ID, const size_t& Size) override;

		void UnMapBufferMemory(const uint32_t& ID) override;

		void RemoveBuffer(const uint32_t& ID) override;

		void GenerateDepthBuffer(const uint32_t& ID, const Vector2D& Size) override;

		void GetTransferStages(const VkImageLayout& OldLayout, const VkImageLayout& NewLayout,
		                       VkImageMemoryBarrier& imageBarrier, VkPipelineStageFlags& srcStage,
		                       VkPipelineStageFlags& dstStage, const VkFormat& Format);

		void TransitionimageLayout(const VkImage& Image, const VkFormat& Format, const VkImageLayout& OldLayout, const VkImageLayout& NewLayout);

		void CopyBufferToImage(const VkBuffer& Buffer, const VkImage& Image, const uint32_t& Width, const uint32_t& Height);

		void CreateImage(const uint32_t& Width, const uint32_t& Height, const VkFormat& Format, const VkImageTiling& Tilling, const
		                 VkImageUsageFlags& Usage, const VkMemoryPropertyFlags& Properties, VkImage& Image, VkDeviceMemory& Memory);
		VkImageView CreateImageView(const VkImage& Image, const VkFormat& Format, const VkImageAspectFlags& AspectFlags);

		void BufferTexture(const uint32_t& ID, const unsigned char* Pixels, const uint32_t& Width, const uint32_t& Height)override;

		BaseDataBuffer* GetBuffer(const uint32_t& ID) override;
		BaseDataBuffer* GetTexture(const uint32_t& ID) override;

		void DrawVertexData(const uint32_t& ID) override;

		std::map<uint32_t, DataBuffer> RegisteredBuffers;

		std::map<uint32_t, TextureBuffer> RegisteredTextures;

		std::map<BufferTargets, VkBufferUsageFlags> TargetToVulkan{ {BufferTargets::VERTEX, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT}, {Indices, VK_BUFFER_USAGE_INDEX_BUFFER_BIT}, {Transfer, VK_BUFFER_USAGE_TRANSFER_DST_BIT} };
		std::map<BufferFormat, VkFormat> FormatToVulkan{ {BufferFormat::Vector2, VK_FORMAT_R32G32_SFLOAT}, {Vector3, VK_FORMAT_R32G32B32_SFLOAT}};
	};

}
