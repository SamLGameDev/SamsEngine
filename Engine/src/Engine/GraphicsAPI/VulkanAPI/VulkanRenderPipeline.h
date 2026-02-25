
#pragma once

#include "ErrorCodes.h"
#include "GraphicsAPIConstructor.h"




namespace Vulkan
{
	class ULogicalDevice;
	class UImageView;
	class Shader;
	class UVulkanComputeShader;
}

namespace Vulkan
{

	class URenderPipeline
	{
	public:

		URenderPipeline(Shader& InShader, const Array<VkDescriptorSetLayoutBinding>& Descriptors);
		URenderPipeline(UVulkanComputeShader& InShader, const Array<VkDescriptorSetLayoutBinding>& Descriptors);
		~URenderPipeline();

		URenderPipeline(const URenderPipeline& Other)
		{
			Copy(Other);
		}

		void Copy(const URenderPipeline& Other)
		{
			Layout = Other.Layout;
			Pipeline = Other.Pipeline;
			DescriptorLayout = Other.DescriptorLayout;
			DescriptorSet = Other.DescriptorSet;
		}

		URenderPipeline& operator=(const URenderPipeline& Other)
		{
			if (this != &Other)
			{
				Copy(Other);
			}
			return *this;
		}

		ErrorCodes Init(Shader& InShader, const Array<VkDescriptorSetLayoutBinding>& Descriptors);

		ErrorCodes Init(UVulkanComputeShader& InShader, const Array<VkDescriptorSetLayoutBinding>& Descriptors);

		ErrorCodes ShutDown();

		const VkPipeline& GetPipeline()const
		{
			return Pipeline;
		}

		const VkDescriptorSet& GetDescriptorSet() const
		{
			return DescriptorSet;
		}

		const VkPipelineLayout& GetPipelineLayout()const
		{
			return Layout;
		}

	private:

		VkPipelineLayout Layout;

		VkPipeline Pipeline;

		VkDescriptorSetLayout DescriptorLayout;

		VkDescriptorSet DescriptorSet;

	};
}
