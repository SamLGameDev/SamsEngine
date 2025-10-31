
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

	private:

		VkPipelineLayout Layout;

	};
}
