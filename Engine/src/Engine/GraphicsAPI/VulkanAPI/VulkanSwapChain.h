
#pragma once

#include "ErrorCodes.h"
#include "GraphicsAPIConstructor.h"


namespace Vulkan
{
	class ULogicalDevice;
	class UImageView;
}

namespace Vulkan
{

	class USwapChain
	{
	public:

		USwapChain(ULogicalDevice* InOwningDevice);
		~USwapChain();

		ErrorCodes Init();
		ErrorCodes CreateSwapChain();
		VkSurfaceFormatKHR ChooseSwapChainFormat(const Array<VkSurfaceFormatKHR>& AvailableFormats);
		VkPresentModeKHR ChooseSwapChainPresent(const Array<VkPresentModeKHR>& AvailablePresents);
		VkExtent2D ChooseSwapChainExtent(const VkSurfaceCapabilitiesKHR& capabilities);
		ErrorCodes CreateImageViews();

		ErrorCodes ShutDown();

		const ULogicalDevice* GetOwningDevice() const
		{
			return OwningDevice;
		}

		VkExtent2D GetSwapChainExtent() const
		{
			return SwapChainExtent;
		}

		VkSurfaceFormatKHR GetSwapChainFormat() const
		{
			return SwapChainFormat;
		}

	private:

		VkSwapchainKHR SwapChain;

		ULogicalDevice* OwningDevice;

		Array<VkImage> SwapChainImages;

		VkSurfaceFormatKHR SwapChainFormat;
		VkExtent2D SwapChainExtent;

		Array<UImageView> SwapChainImageViews;

	};
}
