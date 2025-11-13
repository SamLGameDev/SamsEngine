
#pragma once

#include "ErrorCodes.h"
#include "GraphicsAPIConstructor.h"


namespace Vulkan
{
	class ULogicalDevice;
	class UImageView;
	class Shader;
}

namespace Vulkan
{

	class URenderPipeline
	{
	public:

		URenderPipeline(Shader& InShader);
		~URenderPipeline();

		ErrorCodes Init(Shader& InShader);

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
