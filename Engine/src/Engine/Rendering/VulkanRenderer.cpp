#include "VulkanRenderer.h"
#include "VulkanLogicalDevice.h"
#include "VulkanGraphicsCard.h"
#define GLFW_INCLUDE_VULKAN
#include <complex.h>
#include <GLFW/glfw3.h>

#include "Vector2D.h"
#include "VulkanImageView.h"
#include "VulkanInstance.h"
#include "Window.h"
#include "VulkanImageView.h"


namespace Vulkan
{
	URenderer::URenderer(UGraphicsCard* InOwningCard)
	{
		OwningCard = InOwningCard;
		Init();
	}

	URenderer::~URenderer()
	{
		ShutDown();
	}

	ErrorCodes URenderer::Init()
	{
		CreateRenderPass();

		SInstance::GetInstance()->RenderPass = RenderPass;

		if (CreateCommandPool() == ERROR) return ERROR;

		if (CreateCommandBuffers() == ERROR) return ERROR;

		if (CreateSyncObjects() == ERROR) return ERROR;

		Test = new Shader("triangle", "/Shaders/");

		return SUCCEEDED;
	}

	ErrorCodes URenderer::CreateCommandPool()
	{
		VkCommandPoolCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
		createInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
		createInfo.queueFamilyIndex = OwningCard->GetFoundQueueFamilies().GraphicsFamily.value();

		if (vkCreateCommandPool(*OwningCard->GetLogicalDevice()->GetVulkanLogicalDevice(), &createInfo, nullptr, &CommandPool) != VK_SUCCESS)
		{
			return ERROR;
		}
		return SUCCEEDED;
	}

	ErrorCodes URenderer::CreateCommandBuffers()
	{
		CommandBuffers.Reallocate(MAX_FRAMES_IN_FLIGHT);

		VkCommandBufferAllocateInfo allocateInfo{};
		allocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocateInfo.commandBufferCount = CommandBuffers.GetSize();
		allocateInfo.commandPool = CommandPool;
		allocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		if (vkAllocateCommandBuffers(*OwningCard->GetLogicalDevice()->GetVulkanLogicalDevice(), &allocateInfo, CommandBuffers.GetFirstRef()) != VK_SUCCESS) return ERROR;

		return SUCCEEDED;
	}


	ErrorCodes URenderer::CreateSyncObjects()
	{

		ImageAvailableSemaphores.Reallocate(MAX_FRAMES_IN_FLIGHT);
		RenderFinishedSemaphores.Reallocate(MAX_FRAMES_IN_FLIGHT);
		InFlightFences.Reallocate(MAX_FRAMES_IN_FLIGHT);

		VkSemaphoreCreateInfo semaphoreCreateInfo{};
		semaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

		VkFenceCreateInfo fenceCreateInfo{};
		fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
		fenceCreateInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

		for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
		{
			if (vkCreateSemaphore(*OwningCard->GetLogicalDevice()->GetVulkanLogicalDevice(), &semaphoreCreateInfo, nullptr, ImageAvailableSemaphores.GetItemAtRef(i)) != VK_SUCCESS) return ERROR;

			if (vkCreateSemaphore(*OwningCard->GetLogicalDevice()->GetVulkanLogicalDevice(), &semaphoreCreateInfo, nullptr, RenderFinishedSemaphores.GetItemAtRef(i)) != VK_SUCCESS) return ERROR;
			if (vkCreateFence(*OwningCard->GetLogicalDevice()->GetVulkanLogicalDevice(), &fenceCreateInfo, nullptr, InFlightFences.GetItemAtRef(i)) != VK_SUCCESS) return ERROR;
		}

		return SUCCEEDED;
	}

	void URenderer::Render()
	{
		glfwPollEvents();

		vkWaitForFences(*OwningCard->GetLogicalDevice()->GetVulkanLogicalDevice(), 1, InFlightFences.GetItemAtRef(CurrentFrame), VK_TRUE, UINT64_MAX);

		uint32_t imageIndex;
		VkResult result = vkAcquireNextImageKHR(*OwningCard->GetLogicalDevice()->GetVulkanLogicalDevice(), OwningCard->GetLogicalDevice()->GetSwapChain()->GetSwapChain(), UINT64_MAX, ImageAvailableSemaphores[CurrentFrame], VK_NULL_HANDLE, &imageIndex);

		if (result == VK_ERROR_OUT_OF_DATE_KHR)
		{
			OwningCard->GetLogicalDevice()->GetSwapChain()->RecreateSwapChain();
			return;
		}
		else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
		{
			return;
		}

		vkResetFences(*OwningCard->GetLogicalDevice()->GetVulkanLogicalDevice(), 1, InFlightFences.GetItemAtRef(CurrentFrame));

		vkResetCommandBuffer(CommandBuffers[CurrentFrame], 0);

		CurrentBuffer = CommandBuffers[CurrentFrame];

		RecordCommandBuffer(CommandBuffers[CurrentFrame], imageIndex);

		VkSubmitInfo submitInfo{};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

		VkSemaphore waitSemaphores[] = { ImageAvailableSemaphores[CurrentFrame] };

		VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
		submitInfo.waitSemaphoreCount = 1;
		submitInfo.pWaitSemaphores = waitSemaphores;
		submitInfo.pWaitDstStageMask = waitStages;

		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = CommandBuffers.GetItemAtRef(CurrentFrame);

		VkSemaphore signalSemaphores[] = { RenderFinishedSemaphores[CurrentFrame] };

		submitInfo.signalSemaphoreCount = 1;
		submitInfo.pSignalSemaphores = signalSemaphores;

		if (vkQueueSubmit(OwningCard->GetLogicalDevice()->GetGraphicsQueue(), 1, &submitInfo, InFlightFences[CurrentFrame]) != VK_SUCCESS) return;

		VkPresentInfoKHR presentInfo{};
		presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
		presentInfo.pImageIndices = &imageIndex;

		VkSwapchainKHR swapChains[] = { OwningCard->GetLogicalDevice()->GetSwapChain()->GetSwapChain() };

		presentInfo.pSwapchains = swapChains;
		presentInfo.swapchainCount = 1;
		presentInfo.waitSemaphoreCount = 1;
		presentInfo.pWaitSemaphores = signalSemaphores;
		presentInfo.pResults = nullptr;

		result = vkQueuePresentKHR(OwningCard->GetLogicalDevice()->GetGraphicsQueue(), &presentInfo);

		if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || FirstWindow::HasWindowBeenResized())
		{
			FirstWindow::ResetWindowResize();
			OwningCard->GetLogicalDevice()->GetSwapChain()->RecreateSwapChain();
		}
		else if (result != VK_SUCCESS) return;

		CurrentFrame = (CurrentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
	}

	ErrorCodes URenderer::RecordCommandBuffer(VkCommandBuffer Buffer, std::uint32_t ImageIndex)
	{
		VkCommandBufferBeginInfo beginInfo{};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		beginInfo.pInheritanceInfo = nullptr;
		beginInfo.flags = 0;

		if (vkBeginCommandBuffer(Buffer, &beginInfo) != VK_SUCCESS) return ERROR;

		VkRenderPassBeginInfo renderBeginInfo{};
		renderBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderBeginInfo.renderPass = SInstance::GetInstance()->RenderPass->GetVulkanRenderPass();
		renderBeginInfo.framebuffer = OwningCard->GetLogicalDevice()->GetSwapChain()->GetFrameBuffers().GetItemAtRef(ImageIndex)->GetFrameBuffer();
		renderBeginInfo.renderArea.offset = { 0, 0 };
		renderBeginInfo.renderArea.extent = OwningCard->GetLogicalDevice()->GetSwapChain()->GetSwapChainExtent();
		VkClearValue clearValue = { {{0, 0, 0, 1.f}} };
		renderBeginInfo.clearValueCount = 1;
		renderBeginInfo.pClearValues = &clearValue;

		vkCmdBeginRenderPass(Buffer, &renderBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

		Test->Use();

		VkViewport viewport{};
		viewport.x = 0;
		viewport.y = 0;
		viewport.width = OwningCard->GetLogicalDevice()->GetSwapChain()->GetSwapChainExtent().width;
		viewport.height = OwningCard->GetLogicalDevice()->GetSwapChain()->GetSwapChainExtent().height;
		viewport.minDepth = 0;
		viewport.maxDepth = 1;

		vkCmdSetViewport(Buffer, 0, 1, &viewport);

		VkRect2D scissor;
		scissor.extent = OwningCard->GetLogicalDevice()->GetSwapChain()->GetSwapChainExtent();
		scissor.offset = { 0, 0 };
		vkCmdSetScissor(Buffer, 0, 1, &scissor);

		vkCmdDraw(Buffer, 3, 1, 0, 0);

		vkCmdEndRenderPass(Buffer);

		if (vkEndCommandBuffer(Buffer) != VK_SUCCESS) return ERROR;


		return SUCCEEDED;


	}

	ErrorCodes URenderer::ShutDown()
	{
		for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
		{
			vkDestroySemaphore(*OwningCard->GetLogicalDevice()->GetVulkanLogicalDevice(), ImageAvailableSemaphores.GetItemAt(i), nullptr);

			vkDestroySemaphore(*OwningCard->GetLogicalDevice()->GetVulkanLogicalDevice(), RenderFinishedSemaphores.GetItemAt(i), nullptr);

			vkDestroyFence(*OwningCard->GetLogicalDevice()->GetVulkanLogicalDevice(), InFlightFences.GetItemAt(i), nullptr);
		}

		vkDestroyCommandPool(*OwningCard->GetLogicalDevice()->GetVulkanLogicalDevice(), CommandPool, nullptr);

		delete RenderPass;

		return SUCCEEDED;
	}

	void URenderer::CreateRenderPass()
	{
		RenderPass = new URenderPass();
	}
}
