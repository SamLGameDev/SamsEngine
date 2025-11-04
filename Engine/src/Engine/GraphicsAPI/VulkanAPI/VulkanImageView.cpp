#include "VulkanImageView.h"

#include "VulkanLogicalDevice.h"
#include "VulkanSwapChain.h"

namespace Vulkan
{
	UImageView::UImageView(USwapChain* InOwningSwapChain, VkImageViewCreateInfo& CreateInfo)
	{
		OwningSwapChain = InOwningSwapChain;
		Init(CreateInfo);
	}

	UImageView::~UImageView()
	{
		ShutDown();
	}

	ErrorCodes UImageView::Init(VkImageViewCreateInfo& CreateInfo)
	{
		if (CreateImageView(CreateInfo) == ERROR) return ERROR;
		return SUCCEEDED;
	}

	ErrorCodes UImageView::CreateImageView(VkImageViewCreateInfo& CreateInfo)
	{

		if (vkCreateImageView
		(
			*OwningSwapChain->GetOwningDevice()->GetVulkanLogicalDevice(),
			&CreateInfo,
			nullptr,
			&ImageView
		) != VK_SUCCESS) return ERROR;
		

		return SUCCEEDED;
	}

	ErrorCodes UImageView::ShutDown()
	{
		if (ImageView == VK_NULL_HANDLE) return SUCCEEDED;

		//vkDestroyImageView(*OwningSwapChain->GetOwningDevice()->GetVulkanLogicalDevice(), ImageView, nullptr);
		ImageView = VK_NULL_HANDLE;
		return SUCCEEDED;
	}
}
