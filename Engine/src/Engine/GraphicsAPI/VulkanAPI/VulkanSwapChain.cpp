// DO NOT MARK 
//This is because it has been submitted for my COMP305. Link to Original: https://github.falmouth.ac.uk/GA-Undergrad-Student-Work-25-26/Comp305-Engine-SL295211.git
#include "VulkanSwapChain.h"
#include "VulkanLogicalDevice.h"
#include "VulkanGraphicsCard.h"
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "DataBuffers.h"
#include "DataBuffersVulkan.h"
#include "Vector2D.h"
#include "VulkanImageView.h"
#include "VulkanInstance.h"
#include "Window.h"
#include "VulkanImageView.h"


namespace Vulkan
{
	USwapChain::USwapChain(ULogicalDevice* InOwningDevice)
	{
		OwningDevice = InOwningDevice;
		Init();
	}

	USwapChain::~USwapChain()
	{
		ShutDown();
	}
	ErrorCodes USwapChain::Init()
	{
		if (CreateSwapChain() == ERROR)
		{
			return ERROR;
		}

		if (CreateImageViews() == ERROR) return ERROR;

		return SUCCEEDED;
	}
	ErrorCodes USwapChain::CreateSwapChain()
	{
		const SwapChainSupportDetails& swapChainSupport = OwningDevice->GetOwningCard()->GetSwapChainSupportDetails();

		VkSurfaceFormatKHR format = ChooseSwapChainFormat(swapChainSupport.Formats);
		VkPresentModeKHR present = ChooseSwapChainPresent(swapChainSupport.Presents);
		VkExtent2D extent = ChooseSwapChainExtent(swapChainSupport.Capabilities);

		std::uint32_t imageCount = swapChainSupport.Capabilities.minImageCount + 1;

		if (swapChainSupport.Capabilities.maxImageCount > 0 && imageCount > swapChainSupport.Capabilities.maxImageCount)
		{
			imageCount = swapChainSupport.Capabilities.maxImageCount;
		}

		VkSwapchainCreateInfoKHR createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
		createInfo.imageExtent = extent;
		createInfo.imageFormat = format.format;
		createInfo.presentMode = present;
		createInfo.imageColorSpace = format.colorSpace;
		createInfo.surface = SInstance::GetInstance()->WindowsInterface;
		createInfo.minImageCount = imageCount;
		createInfo.imageArrayLayers = 1;
		createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

		const QueueFamilyIndices& graphicsIndices = OwningDevice->GetOwningCard()->GetFoundQueueFamilies();

		const Array<std::uint32_t> indices = { graphicsIndices.GraphicsFamily.value(), graphicsIndices.PresentFamily.value() };

		if (graphicsIndices.GraphicsFamily != graphicsIndices.PresentFamily)
		{
			createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
			createInfo.queueFamilyIndexCount = static_cast<uint32_t>(indices.GetSize());
			createInfo.pQueueFamilyIndices = indices.GetFirstPtr();
		}
		else
		{
			createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
			createInfo.queueFamilyIndexCount = 0;
			createInfo.pQueueFamilyIndices = nullptr;
		}

		createInfo.preTransform = swapChainSupport.Capabilities.currentTransform;
		createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
		createInfo.clipped = VK_TRUE;
		createInfo.oldSwapchain = VK_NULL_HANDLE;

		if (vkCreateSwapchainKHR(*OwningDevice->GetVulkanLogicalDevice(), &createInfo, nullptr, &SwapChain) != VK_SUCCESS) return ERROR;

		uint32_t swapChainImageCount;

		vkGetSwapchainImagesKHR(*OwningDevice->GetVulkanLogicalDevice(), SwapChain, &swapChainImageCount, nullptr);

		SwapChainImages.Reallocate(swapChainImageCount);

		vkGetSwapchainImagesKHR(*OwningDevice->GetVulkanLogicalDevice(), SwapChain, &swapChainImageCount, SwapChainImages.GetFirstPtr());

		SwapChainExtent = extent;
		SwapChainFormat = format;


		return SUCCEEDED;
	}

	VkSurfaceFormatKHR USwapChain::ChooseSwapChainFormat(const Array<VkSurfaceFormatKHR>& AvailableFormats)
	{
		for (const auto& format : AvailableFormats)
		{
			if (format.format == VK_FORMAT_R8G8B8A8_SRGB && format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
			{
				return format;
			}
		}

		return AvailableFormats[0];
	}

	VkPresentModeKHR USwapChain::ChooseSwapChainPresent(const Array<VkPresentModeKHR>& AvailablePresents)
	{
		for (const auto& present : AvailablePresents)
		{
			if (present == VK_PRESENT_MODE_MAILBOX_KHR)
			{
				return present;
			}
		}

		return VK_PRESENT_MODE_FIFO_KHR;
	}

	VkExtent2D USwapChain::ChooseSwapChainExtent(const VkSurfaceCapabilitiesKHR& capabilities)
	{
		if (capabilities.currentExtent.width != std::numeric_limits<std::uint32_t>::max()) return capabilities.currentExtent;

		std::int32_t x, y;
		glfwGetFramebufferSize(Window::GetWindowInstance()->GetWindow(), &x, &y);

		Vector2D actual = { static_cast<float>(x), static_cast<float>(y) };

		Vector2D min = { capabilities.minImageExtent.width, capabilities.minImageExtent.height };

		Vector2D max = { capabilities.maxImageExtent.width, capabilities.maxImageExtent.height };

		Vector2D::Clamp(actual, min, max);

		return { static_cast<std::uint32_t>(actual.X), static_cast<std::uint32_t>(actual.Y) };

	}

	ErrorCodes USwapChain::CreateImageViews()
	{
		SwapChainImageViews.Reallocate(SwapChainImages.GetSize());
		for (size_t i = 0; i < SwapChainImages.GetSize(); i++)
		{
			VkImageViewCreateInfo createInfo{};
			createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
			createInfo.image = SwapChainImages[i];
			createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
			createInfo.format = SwapChainFormat.format;
			createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
			createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
			createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
			createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
			createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			createInfo.subresourceRange.baseMipLevel = 0;
			createInfo.subresourceRange.levelCount = 1;
			createInfo.subresourceRange.baseArrayLayer = 0;
			createInfo.subresourceRange.layerCount = 1;

			SwapChainImageViews[i] = UImageView(this, createInfo);
		}

		return SUCCEEDED;
	}

	void USwapChain::CreateFrameBuffers()
	{
		FrameBuffers.Reallocate(SwapChainImageViews.GetSize());

		::DataBuffers::GenTexture(DepthBufferID);
		::DataBuffers::GenerateDepthBuffer(DepthBufferID, { SwapChainExtent.width, SwapChainExtent.height });

		TextureBuffer* buffer = dynamic_cast<TextureBuffer*>(::DataBuffers::GetTexture(DepthBufferID));

		for (size_t i = 0; i < SwapChainImageViews.GetSize(); i++)
		{
			Array<VkImageView> attachments = { SwapChainImageViews[i].GetVulkanImageView(), buffer->ImageViews[0] };

			VkFramebufferCreateInfo framebufferCreateInfo{};
			framebufferCreateInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
			framebufferCreateInfo.renderPass = SInstance::GetInstance()->RenderPass->GetVulkanRenderPass();
			framebufferCreateInfo.attachmentCount = static_cast<uint32_t>(attachments.GetSize());
			framebufferCreateInfo.pAttachments = attachments.GetFirstPtr();
			framebufferCreateInfo.width = SwapChainExtent.width;
			framebufferCreateInfo.height = SwapChainExtent.height;
			framebufferCreateInfo.layers = 1;

			FrameBuffers[i] =  UFrameBuffer(framebufferCreateInfo);
		}
	}


	ErrorCodes USwapChain::RecreateSwapChain()
	{
		int width, height = 0;

		glfwGetFramebufferSize(Window::GetWindowInstance()->GetWindow(), &width, &height);

		while (width == 0 || height == 0)
		{

			glfwGetFramebufferSize(Window::GetWindowInstance()->GetWindow(), &width, &height);
			glfwWaitEvents();
		}

		vkDeviceWaitIdle(*OwningDevice->GetVulkanLogicalDevice());

		ShutDown();

		CreateSwapChain();
		CreateImageViews();
		CreateFrameBuffers();

		return SUCCEEDED;
	}

	ErrorCodes USwapChain::ShutDown()
	{

		for (auto& buffer : FrameBuffers)
		{
			buffer.ShutDown();
		}

		for (UImageView view : SwapChainImageViews)
		{
			view.ShutDown();
		}

		vkDestroySwapchainKHR(*OwningDevice->GetVulkanLogicalDevice(), SwapChain, nullptr);

		return SUCCEEDED;
	}
}
