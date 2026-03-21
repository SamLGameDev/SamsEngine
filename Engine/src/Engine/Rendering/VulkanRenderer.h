// DO NOT MARK
//This is because it has been submitted for my COMP305. Link to Original: https://github.falmouth.ac.uk/GA-Undergrad-Student-Work-25-26/Comp305-Engine-SL295211.git
#pragma once

#include "BaseRenderer.h"
#include "ErrorCodes.h"
#include "GraphicsAPIConstructor.h"
#include "Shader.h"
#include "Vector2D.h"
#include "VulkanFrameBuffer.h"
#include "VulkanRenderPass.h"

constexpr std::uint8_t MAX_FRAMES_IN_FLIGHT = 3;

namespace Vulkan
{
	class UGraphicsCard;
}

namespace Vulkan
{
	class ULogicalDevice;
	class UImageView;
}

namespace Vulkan
{

	class URenderer : BaseRenderer
	{
	public:

		URenderer(UGraphicsCard* InOwningCard);
		~URenderer() override;

		URenderer(const URenderer& Other)
		{
			Copy(Other);
		}

		URenderer& operator=(const URenderer& Other)
		{
			if (this == &Other)
			{
				return *this;
			}
			Copy(Other);
			return *this;
		}

		void Copy(const URenderer& InOther)
		{
			if (CommandPool != VK_NULL_HANDLE)ShutDown();

			CommandPool = InOther.CommandPool;
			CommandBuffers = InOther.CommandBuffers;
			ImageAvailableSemaphores = InOther.ImageAvailableSemaphores;
			RenderFinishedSemaphores = InOther.RenderFinishedSemaphores;
			InFlightFences = InOther.InFlightFences;
		}

		URenderer(URenderer&& Other) noexcept
		{
			Move(Other);
		}

		URenderer& operator=(URenderer&& Other) noexcept
		{
			if (this != &Other)
			{
				Move(Other);

			}
			return *this;
		}

		void Move(URenderer& InOther)
		{
			if (CommandPool != VK_NULL_HANDLE)ShutDown();

			CommandPool = InOther.CommandPool;
			CommandBuffers = InOther.CommandBuffers;
			ImageAvailableSemaphores = InOther.ImageAvailableSemaphores;
			RenderFinishedSemaphores = InOther.RenderFinishedSemaphores;
			InFlightFences = InOther.InFlightFences;

			InOther.CommandPool = VK_NULL_HANDLE;
			InOther.CommandBuffers.Empty();
			InOther.ImageAvailableSemaphores.Empty();
			InOther.RenderFinishedSemaphores.Empty();
			InOther.InFlightFences.Empty();
		}

		ErrorCodes Init();

		ErrorCodes CreateCommandPool();
		ErrorCodes CreateCommandBuffers();
		ErrorCodes CreateSyncObjects();

		void Render();
		ErrorCodes RecordCommandBuffer(VkCommandBuffer Buffer, std::uint32_t ImageIndex);

		VkCommandBuffer& GetCurrentBuffer()
		{
			return CurrentBuffer;
		}

		ErrorCodes ShutDown();

		void CreateRenderPass();

		VkCommandPool* GetTransferPool()
		{
			return &TransferPool;
		}

		VkCommandPool* GetComputePool()
		{
			return &ComputePool;
		}

		VkFence& GetCopyFence()
		{
			return CopyFence;
		}

		VkDescriptorPool* GetDescriptorPool()
		{
			return &DescriptorPool;
		}

		VkSampler GetSampler() const
		{
			return Sampler;
		}

		UGraphicsCard* GetOwningCard() const
		{
			return OwningCard;
		}

		void Draw(const size_t& Size) override;

		void WaitForDrawToFinish();

	private:

		VkCommandPool CommandPool;
		Array<VkCommandBuffer> CommandBuffers;

		VkCommandPool TransferPool;

		VkCommandPool ComputePool;

		VkDescriptorPool DescriptorPool;

		UGraphicsCard* OwningCard;

		Array<VkSemaphore> ImageAvailableSemaphores;
		Array<VkSemaphore> RenderFinishedSemaphores;
		Array<VkFence> InFlightFences;

		VkSampler Sampler;

		VkFence CopyFence;

		URenderPass* RenderPass;

		size_t CurrentFrame = 0;

		VkCommandBuffer CurrentBuffer;


	};
}
