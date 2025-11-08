
#pragma once

#include "ErrorCodes.h"
#include "GraphicsAPIConstructor.h"
#include "Shader.h"
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

	class URenderer
	{
	public:

		URenderer(UGraphicsCard* InOwningCard);
		~URenderer();

		URenderer(const URenderer& Other)
		{
			Copy(Other);
		}

		URenderer& operator=(const URenderer& Other)
		{
			Copy(Other);
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

		URenderer(URenderer&& Other)
		{
			Move(std::move(Other));
		}

		URenderer& operator=(URenderer&& Other)
		{
			Move(std::move(Other));
		}

		void Move(URenderer&& InOther)
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

	private:

		VkCommandPool CommandPool;
		Array<VkCommandBuffer> CommandBuffers;

		UGraphicsCard* OwningCard;

		Array<VkSemaphore> ImageAvailableSemaphores;
		Array<VkSemaphore> RenderFinishedSemaphores;
		Array<VkFence> InFlightFences;

		URenderPass* RenderPass;

		Shader* Test;

		size_t CurrentFrame = 0;

		VkCommandBuffer CurrentBuffer;

	};
}
