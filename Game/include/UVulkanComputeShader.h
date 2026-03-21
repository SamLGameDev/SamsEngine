#pragma once
#include <memory>
#include <string>
#include <vulkan/vulkan_core.h>

#include "Array.h"
#include "UBaseComputeShader.h"

namespace Vulkan
{
	class URenderPipeline;

	class UVulkanComputeShader : public UBaseComputeShader
	{
	public:
		UVulkanComputeShader() = default;

		~UVulkanComputeShader();

		UVulkanComputeShader(const std::string_view& InName, const std::string_view& InStorageLocation);
		VkPipelineShaderStageCreateInfo CompileComputeShader() const;
		bool DoesComputeShaderExist() const;
		std::string GetShaderLocation() const;

		void Use() override;

		void Dispatch(const size_t& NumGroupsX, const size_t& NumGroupsY, const size_t& NumGroupsZ) override;

		void WaitForCompletion()const override;
		static std::shared_ptr<UBaseComputeShader> CreateVulkanComputeShader(const std::string_view& InName, const std::string_view& InStorageLocation);

		const VkDescriptorSet& GetDescriptorSet() const;

		const Array<VkPipelineShaderStageCreateInfo>& GetShaderStages() const
		{
			return ShaderStages;
		}
		 URenderPipeline* GetPipeline() const
		{
			return Pipeline.get();
		 }

	private:
		std::string Name;
		std::string StorageLocation;

		Array<VkPipelineShaderStageCreateInfo> ShaderStages;

		std::unique_ptr<URenderPipeline> Pipeline;


		VkCommandBuffer CommandBuffer;

		VkFence ComputeFence = VK_NULL_HANDLE;

	};
}
