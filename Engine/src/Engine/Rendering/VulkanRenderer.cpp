#include "VulkanRenderer.h"
#include "VulkanLogicalDevice.h"
#include "VulkanGraphicsCard.h"
#define GLFW_INCLUDE_VULKAN
#include <chrono>
#include <complex.h>
#include <GLFW/glfw3.h>

#include "CorePaths.h"
#include "DataBuffers.h"
#include "Transform.h"
#include "Vector2D.h"
#include "VulkanImageView.h"
#include "VulkanInstance.h"
#include "Window.h"
#include "VulkanImageView.h"
#include "glm/ext/matrix_clip_space.hpp"
#include "glm/ext/matrix_transform.hpp"
#include "Voronoi.h"
#include "InterfaceRenderer.h"
#include "Voronoi2D.h"
#include "VoronoiClipping.h"

namespace Vulkan
{
	URenderer::URenderer(UGraphicsCard* InOwningCard)
	{
		OwningCard = InOwningCard;
	}

	URenderer::~URenderer()
	{
		ShutDown();
	}

	ErrorCodes URenderer::Init()
	{
		::Renderer::RealRenderer = this;

		CreateRenderPass();

		SInstance::GetInstance()->RenderPass = RenderPass;

		if (CreateCommandPool() == ERROR) return ERROR;

		if (CreateCommandBuffers() == ERROR) return ERROR;

		if (CreateSyncObjects() == ERROR) return ERROR;

		Array<VkDescriptorPoolSize> poolSizes;

		VkDescriptorPoolSize dPoolSize{};
		dPoolSize.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		dPoolSize.descriptorCount = 2;

		poolSizes.Add(dPoolSize);
		dPoolSize.type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		dPoolSize.descriptorCount = 1;
		poolSizes.Add(dPoolSize);

		VkDescriptorPoolCreateInfo dPoolCreateInfo{};
		dPoolCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		dPoolCreateInfo.pPoolSizes = poolSizes.GetFirstPtr();
		dPoolCreateInfo.poolSizeCount = poolSizes.GetSize();
		dPoolCreateInfo.maxSets = 2000000;

		vkCreateDescriptorPool(*OwningCard->GetLogicalDevice()->GetVulkanLogicalDevice(), &dPoolCreateInfo, nullptr, &DescriptorPool);

		VkSamplerCreateInfo samplerCreateInfo{};
		samplerCreateInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
		samplerCreateInfo.magFilter = VK_FILTER_LINEAR;
		samplerCreateInfo.minFilter = VK_FILTER_LINEAR;
		samplerCreateInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		samplerCreateInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		samplerCreateInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		samplerCreateInfo.anisotropyEnable = VK_TRUE;
		samplerCreateInfo.maxAnisotropy = SInstance::GetInstance()->GraphicsCard->GetDeviceProperties().limits.maxSamplerAnisotropy;
		samplerCreateInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
		samplerCreateInfo.unnormalizedCoordinates = VK_FALSE;
		samplerCreateInfo.compareEnable = VK_FALSE;
		samplerCreateInfo.compareOp = VK_COMPARE_OP_ALWAYS;
		samplerCreateInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
		samplerCreateInfo.mipLodBias = 0;
		samplerCreateInfo.minLod = 0;
		samplerCreateInfo.maxLod = 0;

		if (vkCreateSampler(*SInstance::GetInstance()->GraphicsCard->GetLogicalDevice()->GetVulkanLogicalDevice(),
			&samplerCreateInfo, nullptr, &Sampler) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to create texture sampler");
		}

		CurrentBuffer = CommandBuffers[0];

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

		VkCommandPoolCreateInfo transferCreateInfo{};
		transferCreateInfo.flags = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT;
		transferCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
		transferCreateInfo.queueFamilyIndex = SInstance::GetInstance()->GraphicsCard->GetFoundQueueFamilies().GraphicsFamily.value();
		vkCreateCommandPool(*SInstance::GetInstance()->GraphicsCard->GetLogicalDevice()->GetVulkanLogicalDevice(),
			&transferCreateInfo, nullptr, &TransferPool);
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
		if (vkAllocateCommandBuffers(*OwningCard->GetLogicalDevice()->GetVulkanLogicalDevice(), &allocateInfo, CommandBuffers.GetFirstPtr()) != VK_SUCCESS) return ERROR;

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
			if (vkCreateSemaphore(*OwningCard->GetLogicalDevice()->GetVulkanLogicalDevice(), &semaphoreCreateInfo, nullptr, ImageAvailableSemaphores.GetItemAtPtr(i)) != VK_SUCCESS) return ERROR;

			if (vkCreateSemaphore(*OwningCard->GetLogicalDevice()->GetVulkanLogicalDevice(), &semaphoreCreateInfo, nullptr, RenderFinishedSemaphores.GetItemAtPtr(i)) != VK_SUCCESS) return ERROR;
			if (vkCreateFence(*OwningCard->GetLogicalDevice()->GetVulkanLogicalDevice(), &fenceCreateInfo, nullptr, InFlightFences.GetItemAtPtr(i)) != VK_SUCCESS) return ERROR;
		}

		vkCreateFence(*SInstance::GetInstance()->GraphicsCard->GetLogicalDevice()->GetVulkanLogicalDevice(),
			&fenceCreateInfo, nullptr, &CopyFence);

		return SUCCEEDED;
	}

	void URenderer::Render()
	{
		glfwPollEvents();

		vkWaitForFences(*OwningCard->GetLogicalDevice()->GetVulkanLogicalDevice(), 1, InFlightFences.GetItemAtPtr(CurrentFrame), VK_TRUE, UINT64_MAX);

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

		vkResetFences(*OwningCard->GetLogicalDevice()->GetVulkanLogicalDevice(), 1, InFlightFences.GetItemAtPtr(CurrentFrame));

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
		submitInfo.pCommandBuffers = CommandBuffers.GetItemAtPtr(CurrentFrame);

		VkSemaphore signalSemaphores[] = { RenderFinishedSemaphores[imageIndex] };

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
		
		Array<VkClearValue> clearValues(2);
		clearValues[0].color = { 0,0 ,0,1 };
		clearValues[1].depthStencil = { 1, 0 };

		VkRenderPassBeginInfo renderBeginInfo{};
		renderBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderBeginInfo.renderPass = SInstance::GetInstance()->RenderPass->GetVulkanRenderPass();
		renderBeginInfo.framebuffer = OwningCard->GetLogicalDevice()->GetSwapChain()->GetFrameBuffers().GetItemAtPtr(ImageIndex)->GetFrameBuffer();
		renderBeginInfo.renderArea.offset = { 0, 0 };
		renderBeginInfo.renderArea.extent = OwningCard->GetLogicalDevice()->GetSwapChain()->GetSwapChainExtent();
		renderBeginInfo.clearValueCount = clearValues.GetSize();
		renderBeginInfo.pClearValues = clearValues.GetFirstPtr();

		vkCmdBeginRenderPass(Buffer, &renderBeginInfo, VK_SUBPASS_CONTENTS_INLINE);


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

		for (FracturePiece3D* piece : FracturesToRender)
		{
			piece->Draw();
		}

		for (Model* model : ModelsToRender)
		{
			model->Draw();
		}

		for (FracturePiece2D* piece : Fracture2DToRender)
		{
			piece->Draw();
		}
		for (FracturedMeshPiece* shape :MeshFracturePiecesToRender )
		{
			shape->Draw();
		}

		vkCmdEndRenderPass(Buffer);

		if (vkEndCommandBuffer(Buffer) != VK_SUCCESS) return ERROR;


		return SUCCEEDED;


	}

	ErrorCodes URenderer::ShutDown()
	{

		vkDestroySampler(*SInstance::GetInstance()->GraphicsCard->GetLogicalDevice()->GetVulkanLogicalDevice(), Sampler, nullptr);

		for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
		{
			vkDestroySemaphore(*OwningCard->GetLogicalDevice()->GetVulkanLogicalDevice(), ImageAvailableSemaphores.GetItemAt(i), nullptr);

			vkDestroySemaphore(*OwningCard->GetLogicalDevice()->GetVulkanLogicalDevice(), RenderFinishedSemaphores.GetItemAt(i), nullptr);

			vkDestroyFence(*OwningCard->GetLogicalDevice()->GetVulkanLogicalDevice(), InFlightFences.GetItemAt(i), nullptr);
		}
		vkDestroyFence(*OwningCard->GetLogicalDevice()->GetVulkanLogicalDevice(), CopyFence, nullptr);

		vkDestroyDescriptorPool(*OwningCard->GetLogicalDevice()->GetVulkanLogicalDevice(), DescriptorPool, nullptr);
		vkDestroyCommandPool(*OwningCard->GetLogicalDevice()->GetVulkanLogicalDevice(), CommandPool, nullptr);
		vkDestroyCommandPool(*OwningCard->GetLogicalDevice()->GetVulkanLogicalDevice(), TransferPool, nullptr);


		delete RenderPass;

		return SUCCEEDED;
	}

	void URenderer::CreateRenderPass()
	{
		RenderPass = new URenderPass(this);
	}

	void URenderer::Draw(const size_t& Size)
	{
		vkCmdDrawIndexed(CurrentBuffer, static_cast<uint32_t>(Size), 1, 0, 0, 0);
	}

	void URenderer::WaitForDrawToFinish()
	{
		vkDeviceWaitIdle(*GetOwningCard()->GetLogicalDevice()->GetVulkanLogicalDevice());
	}
}
