// DO NOT MARK
//This is because it has been submitted for my COMP305. Link to Original: https://github.falmouth.ac.uk/GA-Undergrad-Student-Work-25-26/Comp305-Engine-SL295211.git
#pragma once

#include "ErrorCodes.h"
#include "GraphicsAPIConstructor.h"


namespace Vulkan
{
	class URenderer;
	class ULogicalDevice;
	class UImageView;
}

namespace Vulkan
{

	class URenderPass
	{
	public:

		URenderPass();

		URenderPass(URenderer* InOwningRenderer);

		~URenderPass();
		ErrorCodes CreateRenderPass();

		ErrorCodes Init();

		ErrorCodes ShutDown();

		VkRenderPass GetVulkanRenderPass() const
		{
			return RenderPass;
		}

	private:

		VkRenderPass RenderPass;

		URenderer* OwningRenderer;

	};
}
