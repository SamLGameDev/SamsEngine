
#pragma once

#include "ErrorCodes.h"
#include "GraphicsAPIConstructor.h"
#include "VulkanSwapChain.h"

namespace Vulkan
{
	class UGraphicsCard;

	class ULogicalDevice
	{
	public:

		ULogicalDevice(UGraphicsCard* InOwningCard);
		~ULogicalDevice();

		ErrorCodes Init();
		ErrorCodes CreateLogicalDevice();

		ErrorCodes ShutDown();

		const UGraphicsCard* GetOwningCard() const
		{
			return OwningCard;
		}

		const VkDevice* GetVulkanLogicalDevice() const
		{
			return &LogicalDevice;
		}

		const USwapChain* GetSwapChain() const
		{
			return SwapChain;
		}


	private:

		UGraphicsCard* OwningCard;

		VkDevice LogicalDevice;

		VkQueue GraphicsQueue;
		VkQueue PresentQueue;

		USwapChain* SwapChain;

	};
}
