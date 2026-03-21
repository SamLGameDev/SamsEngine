
// DO NOT MARK
//This is because it has been submitted for my COMP305. Link to Original: https://github.falmouth.ac.uk/GA-Undergrad-Student-Work-25-26/Comp305-Engine-SL295211.git
#include "VulkanRenderPass.h"
#include "VulkanLogicalDevice.h"
#include "VulkanGraphicsCard.h"
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "Vector2D.h"
#include "VulkanImageView.h"
#include "VulkanInstance.h"
#include "Window.h"
#include "VulkanImageView.h"
#include "Renderer.h"


namespace Vulkan
{
	URenderPass::URenderPass()
	{
		Init();
	}

	URenderPass::URenderPass(URenderer* InOwningRenderer)
	{
		OwningRenderer = InOwningRenderer;
		Init();
	}

	URenderPass::~URenderPass()
	{
		ShutDown();
	}

	ErrorCodes URenderPass::CreateRenderPass()
	{
		VkAttachmentDescription colorAttachment{};
		colorAttachment.format = SInstance::GetInstance()->GraphicsCard->GetLogicalDevice()->GetSwapChain()->GetSwapChainFormat().format;
		colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
		colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

		VkAttachmentReference colorAttachmentRef{};
		colorAttachmentRef.attachment = 0;
		colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

		VkAttachmentDescription depthAttachment{};
		depthAttachment.format = OwningRenderer->GetOwningCard()->FindDepthFormat();
		depthAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
		depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

		VkAttachmentReference depthAttachmentRef{};
		depthAttachmentRef.attachment = 1;
		depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;


		VkSubpassDescription subpassDescription{};
		subpassDescription.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
		subpassDescription.colorAttachmentCount = 1;
		subpassDescription.pColorAttachments = &colorAttachmentRef;
		subpassDescription.pDepthStencilAttachment = &depthAttachmentRef;

		VkSubpassDependency subpassDependency{};
		subpassDependency.srcSubpass = VK_SUBPASS_EXTERNAL;
		subpassDependency.dstSubpass = 0;
		subpassDependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
		subpassDependency.srcAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
		subpassDependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
		subpassDependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

		Array<VkAttachmentDescription> attachments = {colorAttachment, depthAttachment};

		VkRenderPassCreateInfo renderPassCreateInfo{};
		renderPassCreateInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
		renderPassCreateInfo.attachmentCount = attachments.GetSize();
		renderPassCreateInfo.pAttachments = attachments.GetFirstPtr();
		renderPassCreateInfo.subpassCount = 1;
		renderPassCreateInfo.pSubpasses = &subpassDescription;
		renderPassCreateInfo.dependencyCount = 1;
		renderPassCreateInfo.pDependencies = &subpassDependency;

		if (vkCreateRenderPass
		(*SInstance::GetInstance()->GraphicsCard->GetLogicalDevice()->GetVulkanLogicalDevice(),
			&renderPassCreateInfo,
			nullptr,
			&RenderPass
		) != VK_SUCCESS) return ERROR;

		return SUCCEEDED;
	}

	ErrorCodes URenderPass::Init()
	{
		if (CreateRenderPass() == ERROR) return ERROR;
		return SUCCEEDED;
	}

	ErrorCodes URenderPass::ShutDown()
	{
		vkDestroyRenderPass(*SInstance::GetInstance()->GraphicsCard->GetLogicalDevice()->GetVulkanLogicalDevice(), RenderPass, nullptr);
		return SUCCEEDED;
	}
}
