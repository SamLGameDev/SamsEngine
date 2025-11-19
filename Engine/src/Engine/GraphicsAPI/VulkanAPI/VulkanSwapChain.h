
#pragma once

#include "ErrorCodes.h"
#include "GraphicsAPIConstructor.h"
#include "VulkanFrameBuffer.h"


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
		void CreateFrameBuffers();
		ErrorCodes RecreateSwapChain();

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

		const VkSwapchainKHR& GetSwapChain() const
		{
			return SwapChain;
		}

		const Array<UFrameBuffer>& GetFrameBuffers()const
		{
			return FrameBuffers;
		}

		Array<UImageView>& GetSwapChainImageViews()
		{
			return SwapChainImageViews;
		}
	private:

		VkSwapchainKHR SwapChain;

		ULogicalDevice* OwningDevice;

		Array<VkImage> SwapChainImages;

		VkSurfaceFormatKHR SwapChainFormat;
		VkExtent2D SwapChainExtent;

		Array<UImageView> SwapChainImageViews;

		Array<UFrameBuffer> FrameBuffers;

		uint32_t DepthBufferID;

	};
}
