#include "UVulkanComputeShader.h"

#include <iostream>

#include "CorePaths.h"
#include "FileSaving.h"
#include "VulkanInstance.h"
#include "VulkanLogicalDevice.h"
#include "VulkanRenderPipeline.h"
#include <Vulkan/vulkan_core.h>

namespace Vulkan
{
	UVulkanComputeShader::~UVulkanComputeShader()
	{
		vkDeviceWaitIdle(*SInstance::GetInstance()->GraphicsCard->GetLogicalDevice()->GetVulkanLogicalDevice());
		vkQueueWaitIdle(SInstance::GetInstance()->GraphicsCard->GetLogicalDevice()->GetComputeQueue());
		vkDestroyFence(*SInstance::GetInstance()->GraphicsCard->GetLogicalDevice()->GetVulkanLogicalDevice(), ComputeFence, nullptr);
		vkFreeCommandBuffers(*SInstance::GetInstance()->GraphicsCard->GetLogicalDevice()->GetVulkanLogicalDevice(),
			*SInstance::GetInstance()->GraphicsCard->GetRenderer()->GetComputePool(), 1, &CommandBuffer);
	}

	UVulkanComputeShader::UVulkanComputeShader(const std::string_view& InName,
	                                           const std::string_view& InStorageLocation)
	{
		Name = InName;
		StorageLocation = InStorageLocation;

		if (!DoesComputeShaderExist())
		{
			std::cerr << "ERROR::SHADER::COMPUTE::" << Name << "::SHADER NOT CREATED" << '\n';
			return;
		}

		ShaderStages.Add(CompileComputeShader());

		Array<VkDescriptorSetLayoutBinding> descriptors;

		VkDescriptorSetLayoutBinding descriptor{};
		descriptor.binding = 1;
		descriptor.descriptorCount = 1;
		descriptor.pImmutableSamplers = nullptr;
		descriptor.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
		descriptor.stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;
		descriptors.Add(descriptor);

		for (size_t i = 3; i < 7; i++)
		{
			VkDescriptorSetLayoutBinding descriptor{};
			descriptor.binding = i;
			descriptor.descriptorCount = 1;
			descriptor.pImmutableSamplers = nullptr;
			descriptor.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
			descriptor.stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;
			descriptors.Add(descriptor);
		}

		Pipeline = std::make_unique<URenderPipeline>(*this, descriptors);

		for (const VkPipelineShaderStageCreateInfo& stage : ShaderStages)
		{
			vkDestroyShaderModule(*SInstance::GetInstance()->GraphicsCard->GetLogicalDevice()->GetVulkanLogicalDevice(), stage.module, nullptr);
		}

		VkCommandBufferAllocateInfo allocateInfo{};
		allocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocateInfo.commandBufferCount = 1;
		allocateInfo.commandPool = *SInstance::GetInstance()->GraphicsCard->GetRenderer()->GetComputePool();
		allocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		vkAllocateCommandBuffers(*SInstance::GetInstance()->GraphicsCard->GetLogicalDevice()->GetVulkanLogicalDevice(), &allocateInfo, &CommandBuffer);

		VkFenceCreateInfo fenceInfo{};
		fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
		fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;
		vkCreateFence(*SInstance::GetInstance()->GraphicsCard->GetLogicalDevice()->GetVulkanLogicalDevice(), &fenceInfo, nullptr, &ComputeFence);

	}

	VkPipelineShaderStageCreateInfo UVulkanComputeShader::CompileComputeShader() const
	{

		const std::string CodeString = UFileWriter::ReadFileContents(GetShaderLocation(), std::ios::binary);

		VkShaderModuleCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		createInfo.codeSize = CodeString.size();
		createInfo.pCode = reinterpret_cast<const uint32_t*>(CodeString.data());

		VkShaderModule compShader;

		vkCreateShaderModule(*SInstance::GetInstance()->GraphicsCard->GetLogicalDevice()->GetVulkanLogicalDevice(), &createInfo, nullptr, &compShader);

		VkPipelineShaderStageCreateInfo vertPipelineCreateInfo{};
		vertPipelineCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		vertPipelineCreateInfo.stage = VK_SHADER_STAGE_COMPUTE_BIT;
		vertPipelineCreateInfo.module = compShader;
		vertPipelineCreateInfo.pName = "main";

		return vertPipelineCreateInfo;
	}


	bool UVulkanComputeShader::DoesComputeShaderExist() const
	{
		struct stat Buffer;

		const bool bDoesFileExist = stat(GetShaderLocation().data(), &Buffer) == 0;

		return bDoesFileExist;
	}

	std::string UVulkanComputeShader::GetShaderLocation() const
	{

		return CorePaths::Contents.Path + "/" + StorageLocation + Name + ".spv";

	}

	void UVulkanComputeShader::Use()
	{
		SInstance::GetInstance()->ActiveComputeShader = this;
	}

	void UVulkanComputeShader::Dispatch(const size_t& NumGroupsX, const size_t& NumGroupsY,
	                                    const size_t& NumGroupsZ)
	{
		vkDeviceWaitIdle(*SInstance::GetInstance()->GraphicsCard->GetLogicalDevice()->GetVulkanLogicalDevice());
		vkWaitForFences(*SInstance::GetInstance()->GraphicsCard->GetLogicalDevice()->GetVulkanLogicalDevice(), 1, &ComputeFence, VK_TRUE, UINT64_MAX);
		vkResetFences(*SInstance::GetInstance()->GraphicsCard->GetLogicalDevice()->GetVulkanLogicalDevice(), 1, &ComputeFence);
		vkResetCommandBuffer(CommandBuffer, 0);
		VkCommandBufferBeginInfo beginInfo{};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		
		vkBeginCommandBuffer(CommandBuffer, &beginInfo);

		vkCmdBindPipeline(CommandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, Pipeline->GetPipeline());
		vkCmdBindDescriptorSets(CommandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, Pipeline->GetPipelineLayout(),
			0, 1, &Pipeline->GetDescriptorSet(), 0, nullptr);

		vkCmdDispatch(CommandBuffer, static_cast<uint32_t>(NumGroupsX), static_cast<uint32_t>(NumGroupsY), static_cast<uint32_t>(NumGroupsZ));

		if (vkEndCommandBuffer(CommandBuffer) != VK_SUCCESS) {
			throw std::runtime_error("failed to record command buffer!");
		}

		VkSubmitInfo submitInfo{};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

		submitInfo.waitSemaphoreCount = 0;

		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &CommandBuffer;

		submitInfo.signalSemaphoreCount = 0;
		submitInfo.pSignalSemaphores = nullptr;
		VkResult result = vkQueueSubmit(SInstance::GetInstance()->GraphicsCard->GetLogicalDevice()->GetComputeQueue(), 1, &submitInfo, ComputeFence);
		if (result != VK_SUCCESS)
		{
			std::cerr << "Failed to submit compute command buffer" << result <<'\n';
		}
	}

	void UVulkanComputeShader::WaitForCompletion() const
	{
		vkWaitForFences(*SInstance::GetInstance()->GraphicsCard->GetLogicalDevice()->GetVulkanLogicalDevice(),
			1, &ComputeFence, VK_TRUE, UINT64_MAX);
	}

	std::shared_ptr<UBaseComputeShader> UVulkanComputeShader::CreateVulkanComputeShader(const std::string_view& InName,
		const std::string_view& InStorageLocation)
	{
		std::shared_ptr<UVulkanComputeShader> shader = std::make_shared<UVulkanComputeShader>(InName, InStorageLocation);

		return shader;
	}

	const VkDescriptorSet& UVulkanComputeShader::GetDescriptorSet() const
	{
		return Pipeline->GetDescriptorSet();
	}
}
