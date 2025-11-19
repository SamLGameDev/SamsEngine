
#pragma once

#include "ErrorCodes.h"
#include "GraphicsAPIConstructor.h"


namespace Vulkan
{
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
