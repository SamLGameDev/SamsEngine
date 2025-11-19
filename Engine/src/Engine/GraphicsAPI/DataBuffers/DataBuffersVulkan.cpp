#include "DataBuffersVulkan.h"

#include <iostream>
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

	TextureBuffer::~TextureBuffer()
	{
		for (auto& imageView : ImageViews)
		{
			vkDestroyImageView(*SInstance::GetInstance()->GraphicsCard->GetLogicalDevice()->GetVulkanLogicalDevice(), imageView, nullptr);
		}

		for (auto& image : Images)
		{
			vkDestroyImage(*SInstance::GetInstance()->GraphicsCard->GetLogicalDevice()->GetVulkanLogicalDevice(), image, nullptr);
		}

		for (auto& memory : BufferMemory)
		{
			vkFreeMemory(*SInstance::GetInstance()->GraphicsCard->GetLogicalDevice()->GetVulkanLogicalDevice(), memory, nullptr);
		}

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

	void DataBuffers::GenTexture(const size_t& Number, Array<uint32_t>& IDs)
	{
		for (size_t i = 0; i < Number; i++)
		{
			uint32_t id;
			do
			{
				id = MathCore::RandomRange<uint32_t>(0, std::numeric_limits<uint32_t>::max());
			} while (RegisteredTextures.contains(id));
			RegisteredTextures.insert({ id, TextureBuffer() });
			IDs.Add(id);
		}
	}

	void DataBuffers::GenTexture(uint32_t& ID)
	{
		do
		{
			ID = MathCore::RandomRange<uint32_t>(0, std::numeric_limits<uint32_t>::max());
		} while (RegisteredTextures.contains(ID));
		RegisteredTextures.insert({ ID, TextureBuffer() });
	}

	void Vulkan::DataBuffers::BindVertexInfo(const uint32_t& ID, const size_t& Location, const size_t& Size,
	                                         const size_t& Stride, const size_t& Offset, const BufferFormat& Format)
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
		attributeDescription.format = FormatToVulkan.at(Format);
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
		try
		{
			DataBuffer& buffer = RegisteredBuffers.at(ID);

			VkDeviceMemory stagingMemory;

			VkBuffer stagingBuffer = CreateBuffer(Size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingMemory);


			void* data;
			vkMapMemory(*SInstance::GetInstance()->GraphicsCard->GetLogicalDevice()->GetVulkanLogicalDevice(), stagingMemory, 0, Size, 0, &data);
			memcpy(data, Data, Size);
			vkUnmapMemory(*SInstance::GetInstance()->GraphicsCard->GetLogicalDevice()->GetVulkanLogicalDevice(), stagingMemory);
			VkDeviceMemory destinationMemory;
			buffer.Buffers.Add(CreateBuffer(Size, TargetToVulkan.at(Transfer) | TargetToVulkan.at(Target), VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, destinationMemory));

			buffer.BufferMemory.Add(destinationMemory);

			CopyBuffer(stagingBuffer, *buffer.Buffers.GetLastPtr(), Size);


			vkDestroyBuffer(*SInstance::GetInstance()->GraphicsCard->GetLogicalDevice()->GetVulkanLogicalDevice(), stagingBuffer, nullptr);
			vkFreeMemory(*SInstance::GetInstance()->GraphicsCard->GetLogicalDevice()->GetVulkanLogicalDevice(), stagingMemory, nullptr);
		}
		catch (const std::exception& error)
		{
			std::cerr << error.what();
		}

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
		buffer.IndexBuffer = CreateBuffer(Size, TargetToVulkan.at(Transfer) | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, destinationMemory);

		buffer.IndexMemory = destinationMemory;

		CopyBuffer(stagingBuffer, buffer.IndexBuffer, Size);


		vkDestroyBuffer(*SInstance::GetInstance()->GraphicsCard->GetLogicalDevice()->GetVulkanLogicalDevice(), stagingBuffer, nullptr);
		vkFreeMemory(*SInstance::GetInstance()->GraphicsCard->GetLogicalDevice()->GetVulkanLogicalDevice(), stagingMemory, nullptr);
	}

	VkCommandPool* DataBuffers::BeginSingleTimeCommands(VkCommandBuffer& CBuffer)
	{
		VkCommandPool* transferPool = SInstance::GetInstance()->GraphicsCard->GetRenderer()->GetTransferPool();

		VkCommandBufferAllocateInfo allocInfo{};
		allocInfo.commandBufferCount = 1;
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocInfo.commandPool = *transferPool;

		vkAllocateCommandBuffers(*SInstance::GetInstance()->GraphicsCard->GetLogicalDevice()->GetVulkanLogicalDevice(), &allocInfo, &CBuffer);

		VkCommandBufferBeginInfo beginInfo{};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
		vkBeginCommandBuffer(CBuffer, &beginInfo);
		return transferPool;
	}

	void DataBuffers::EndSingleTimeCommands(VkCommandBuffer CBuffer, VkCommandPool* TransferPool)
	{
		vkEndCommandBuffer(CBuffer);

		VkSubmitInfo sInfo{};
		sInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		sInfo.commandBufferCount = 1;
		sInfo.pCommandBuffers = &CBuffer;

		VkFence& copyFence = SInstance::GetInstance()->GraphicsCard->GetRenderer()->GetCopyFence();

		vkResetFences(*SInstance::GetInstance()->GraphicsCard->GetLogicalDevice()->GetVulkanLogicalDevice(), 1, &copyFence);

		vkQueueSubmit(SInstance::GetInstance()->GraphicsCard->GetLogicalDevice()->GetGraphicsQueue(), 1, &sInfo, copyFence);
		vkWaitForFences(*SInstance::GetInstance()->GraphicsCard->GetLogicalDevice()->GetVulkanLogicalDevice(), 1, &copyFence, VK_TRUE, UINT64_MAX);

		vkFreeCommandBuffers(*SInstance::GetInstance()->GraphicsCard->GetLogicalDevice()->GetVulkanLogicalDevice(), *TransferPool, 1, &CBuffer);
	}

	void DataBuffers::CopyBuffer(const VkBuffer& SrcBuffer, VkBuffer& DstBuffer, const VkDeviceSize& Size)
	{
		VkCommandBuffer cBuffer;
		VkCommandPool* transferPool = BeginSingleTimeCommands(cBuffer);

		VkBufferCopy copyBuffer{};
		copyBuffer.srcOffset = 0;
		copyBuffer.dstOffset = 0;
		copyBuffer.size = Size;
		vkCmdCopyBuffer(cBuffer, SrcBuffer, DstBuffer, 1, &copyBuffer);

		EndSingleTimeCommands(cBuffer, transferPool);
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

	void DataBuffers::GenerateDepthBuffer(const uint32& ID, const Vector2D& Size)
	{

		TextureBuffer& buffer = RegisteredTextures.at(ID);

		VkImage image;
		VkDeviceMemory = memory;


		VkFormat format = SInstance::GetInstance()->GraphicsCard->FindDepthFormat();
		CreateImage(Size.X, Size.Y, format, VK_IMAGE_TILLING_OPTIMAL, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, image, memory);

		TransitionimageLayout(image, format, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL)


		buffer.ImageViews.Add(CreateImageView(image, format, VK_IMAGE_ASPECT_DEPTH_BIT));
		buffer.Images.Add(image);
		buffer.BufferMemory.Add(memory);




	}

	void DataBuffers::GetTransferStages(const VkImageLayout& OldLayout, const VkImageLayout& NewLayout, VkImageMemoryBarrier& imageBarrier, VkPipelineStageFlags
	                                    & srcStage, VkPipelineStageFlags& dstStage, const VkFormat& Format)
	{
		if (OldLayout == VK_IMAGE_LAYOUT_UNDEFINED && NewLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL)
		{
			imageBarrier.srcAccessMask = 0;
			imageBarrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

			srcStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
			dstStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
			return;
		}

		if (OldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && NewLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
		{
			imageBarrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
			imageBarrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

			srcStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
			dstStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
			return;
		}

		if (NewLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIl_ATTACHMENT_OPYIMAL)
		{
			imageBarrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
			imageBarrier.srcAccessMask = 0;
			imageBarrier.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTCHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
			srcStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
			dstStage = VK_PIPELINE_STATE_EARLY_FRAGMENT_TESTS_BIT;

			if (SInstance::GetInstance()->GraphicsCard->HadStencilAttachment(Format))
			{
				imageBarrier.aspectMask |= VK_IMAGE_ASPECT_STENCIl_BIT;
			}


		}

		throw std::runtime_error("Unsupported Barrier layout");
	}

	void DataBuffers::TransitionimageLayout(const VkImage& Image, const VkFormat& Format,
	                                        const VkImageLayout& OldLayout, const VkImageLayout& NewLayout)
	{
		VkCommandBuffer cBuffer;
		VkCommandPool* pool = BeginSingleTimeCommands(cBuffer);

		VkImageMemoryBarrier imageBarrier{};
		imageBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
		imageBarrier.image = Image;
		imageBarrier.newLayout = NewLayout;
		imageBarrier.oldLayout = OldLayout;
		imageBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		imageBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		imageBarrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		imageBarrier.subresourceRange.layerCount = 1;
		imageBarrier.subresourceRange.baseArrayLayer = 0;
		imageBarrier.subresourceRange.levelCount = 1;
		imageBarrier.subresourceRange.baseMipLevel = 0;

		VkPipelineStageFlags srcStage;
		VkPipelineStageFlags dstStage;

		GetTransferStages(OldLayout, NewLayout, imageBarrier, srcStage, dstStage);

		vkCmdPipelineBarrier(cBuffer, srcStage, dstStage,
			0, 0,
			nullptr, 0,
			nullptr, 1, &imageBarrier);

		EndSingleTimeCommands(cBuffer, pool);
	}

	void DataBuffers::CopyBufferToImage(const VkBuffer& Buffer, const VkImage& Image, const uint32_t& Width,
		const uint32_t& Height)
	{
		VkCommandBuffer cBuffer;
		VkCommandPool* pool = BeginSingleTimeCommands(cBuffer);

		VkBufferImageCopy bufImgCopy{};
		bufImgCopy.bufferOffset = 0;
		bufImgCopy.bufferImageHeight = 0;
		bufImgCopy.bufferRowLength = 0;
		bufImgCopy.imageSubresource.baseArrayLayer = 0;
		bufImgCopy.imageSubresource.mipLevel = 0;
		bufImgCopy.imageSubresource.layerCount = 1;
		bufImgCopy.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;

		bufImgCopy.imageOffset = { 0, 0, 0 };
		bufImgCopy.imageExtent = { Width, Height, 1 };

		vkCmdCopyBufferToImage(cBuffer, Buffer, Image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &bufImgCopy);

		EndSingleTimeCommands(cBuffer, pool);
	}

	void DataBuffers::CreateImage(const uint32_t& Width, const uint32_t& Height, const VkFormat& Format, const VkImageTiling& Tilling, const
	                              VkImageUsageFlags& Usage, const VkMemoryPropertyFlags& Properties, VkImage& Image, VkDeviceMemory& Memory)
	{
		VkImageCreateInfo imageCreateInfo{};
		imageCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
		imageCreateInfo.imageType = VK_IMAGE_TYPE_2D;
		imageCreateInfo.extent = { Width, Height, 1 };
		imageCreateInfo.mipLevels = 1;
		imageCreateInfo.arrayLayers = 1;
		imageCreateInfo.format = Format;
		imageCreateInfo.tiling = Tilling;
		imageCreateInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		imageCreateInfo.usage = Usage;
		imageCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		imageCreateInfo.samples = VK_SAMPLE_COUNT_1_BIT;
		imageCreateInfo.flags = 0;

		if (vkCreateImage(*SInstance::GetInstance()->GraphicsCard->GetLogicalDevice()->GetVulkanLogicalDevice()
		                  , &imageCreateInfo, nullptr, &Image) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to create texture image");
		};

		VkMemoryRequirements imageMemoryRequirements;
		vkGetImageMemoryRequirements(*SInstance::GetInstance()->GraphicsCard->GetLogicalDevice()->GetVulkanLogicalDevice(), Image, &imageMemoryRequirements);

		VkMemoryAllocateInfo memoryAllocateInfo{};
		memoryAllocateInfo.allocationSize = imageMemoryRequirements.size;
		memoryAllocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		memoryAllocateInfo.memoryTypeIndex = SInstance::GetInstance()->GraphicsCard->FindMemoryType(
			imageMemoryRequirements.memoryTypeBits, Properties);
		if (vkAllocateMemory(*SInstance::GetInstance()->GraphicsCard->GetLogicalDevice()->GetVulkanLogicalDevice(),
		                     &memoryAllocateInfo, nullptr, &Memory))
		{
			throw std::runtime_error("Failed to allocate memory");
		}

		vkBindImageMemory(*SInstance::GetInstance()->GraphicsCard->GetLogicalDevice()->GetVulkanLogicalDevice(), Image, Memory, 0);
	}

	VkImageView DataBuffers::CreateImageView(const VkImage& Image, const VkFormat& Format. const VkImageAspectFlags& AspectFlags)
	{
		VkImageViewCreateInfo imageViewCreateInfo{};
		imageViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		imageViewCreateInfo.image = Image;
		imageViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		imageViewCreateInfo.format = Format;
		imageViewCreateInfo.subresourceRange.aspectMask = AspectFlags;
		imageViewCreateInfo.subresourceRange.baseArrayLayer = 0;
		imageViewCreateInfo.subresourceRange.baseMipLevel = 0;
		imageViewCreateInfo.subresourceRange.layerCount = 1;
		imageViewCreateInfo.subresourceRange.levelCount = 1;
		imageViewCreateInfo.flags = 0;

		VkImageView iView;
		if (vkCreateImageView(*SInstance::GetInstance()->GraphicsCard->GetLogicalDevice()->GetVulkanLogicalDevice(),
			&imageViewCreateInfo, nullptr, &iView) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to create image view");
		}

		return iView;

	}

	void DataBuffers::BufferTexture(const uint32_t& ID, const unsigned char* Pixels, const uint32_t& Width, const uint32_t& Height)
	{
		TextureBuffer& texture = RegisteredTextures.at(ID);

		const uint32_t size = Width * Height * 4;

		VkDeviceMemory stagingMemory;

		VkBuffer stagingBuffer = CreateBuffer(size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingMemory);

		void* data;
		vkMapMemory(*SInstance::GetInstance()->GraphicsCard->GetLogicalDevice()->GetVulkanLogicalDevice(), stagingMemory, 0, size, 0, &data);
		memcpy(data, Pixels, size);
		vkUnmapMemory(*SInstance::GetInstance()->GraphicsCard->GetLogicalDevice()->GetVulkanLogicalDevice(), stagingMemory);

		VkImage image;
		VkDeviceMemory memory;

		CreateImage(Width, Height, VK_FORMAT_R8G8B8A8_SRGB,
			VK_IMAGE_TILING_OPTIMAL, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, image, memory);

		TransitionimageLayout(image, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
		CopyBufferToImage(stagingBuffer, image, Width, Height);
		TransitionimageLayout(image, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

		vkDestroyBuffer(*SInstance::GetInstance()->GraphicsCard->GetLogicalDevice()->GetVulkanLogicalDevice(), stagingBuffer, nullptr);
		vkFreeMemory(*SInstance::GetInstance()->GraphicsCard->GetLogicalDevice()->GetVulkanLogicalDevice(), stagingMemory, nullptr);

		texture.ImageViews.Add(CreateImageView(image, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_ASPECT_COLOR_BIT));

		texture.Images.Add(image);
		texture.BufferMemory.Add(memory);
	}

	BaseDataBuffer* DataBuffers::GetBuffer(const uint32_t& ID)
	{
		return &RegisteredBuffers.at(ID);
	}

	BaseDataBuffer* DataBuffers::GetTexture(const uint32_t& ID)
	{
		return &RegisteredTextures.at(ID);
	}

	void Vulkan::DataBuffers::DrawVertexData(const uint32_t& ID)
	{
		const DataBuffer& buffer = RegisteredBuffers.at(ID);

		VkDeviceSize offsets[] = { 0, 0, 0 };
		vkCmdBindVertexBuffers(SInstance::GetInstance()->GraphicsCard->GetRenderer()->GetCurrentBuffer(),
			0, buffer.Buffers.GetSize(), buffer.Buffers.GetFirstRef(), offsets);

		vkCmdBindIndexBuffer(SInstance::GetInstance()->GraphicsCard->GetRenderer()->GetCurrentBuffer(), buffer.IndexBuffer, 0, VK_INDEX_TYPE_UINT16);
	}
}
