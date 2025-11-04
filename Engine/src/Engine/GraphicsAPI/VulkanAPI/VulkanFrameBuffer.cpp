#include "VulkanFrameBuffer.h"
#include "VulkanLogicalDevice.h"
#include "VulkanGraphicsCard.h"
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "Vector2D.h"
#include "VulkanImageView.h"
#include "VulkanInstance.h"
#include "Window.h"
#include "VulkanImageView.h"


namespace Vulkan
{

	UFrameBuffer::UFrameBuffer(VkFramebufferCreateInfo CreateInfo)
	{
		Init(CreateInfo);
	}

	UFrameBuffer::~UFrameBuffer()
	{
		ShutDown();
	}

	void UFrameBuffer::Copy(const UFrameBuffer& Copy)
	{
		if (FrameBuffer != VK_NULL_HANDLE) ShutDown();
		FrameBuffer = Copy.FrameBuffer;
	}

	ErrorCodes UFrameBuffer::Init(VkFramebufferCreateInfo CreateInfo)
	{
		if (vkCreateFramebuffer(*SInstance::GetInstance()->GraphicsCard->GetLogicalDevice()->GetVulkanLogicalDevice(), &CreateInfo, nullptr, &FrameBuffer) != VK_SUCCESS) return ERROR;
		return SUCCEEDED;
	}

	ErrorCodes UFrameBuffer::ShutDown()
	{
		if (FrameBuffer == VK_NULL_HANDLE) return SUCCEEDED;

		//vkDestroyFramebuffer(*SInstance::GetInstance()->GraphicsCard->GetLogicalDevice()->GetVulkanLogicalDevice(), FrameBuffer, nullptr);
		FrameBuffer = VK_NULL_HANDLE;
		return SUCCEEDED;
	}
}
