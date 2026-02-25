
#pragma once

#include "ErrorCodes.h"
#include "FirstWindow.h"
#include "GraphicsAPIConstructor.h"
#include <optional>

#include "VulkanGraphicsCard.h"
#include "VulkanRenderPass.h"


namespace Vulkan
{
	class UVulkanComputeShader;
	class SInstance
	{
	public:


		static SInstance* GetInstance();

		VkInstance VulkanInstance;

		VkSurfaceKHR WindowsInterface;

		UGraphicsCard* GraphicsCard;

		URenderPass* RenderPass;

		UVulkanComputeShader* ActiveComputeShader;

		ErrorCodes ShutDown();

	private:

		SInstance() = default;

		static std::optional<SInstance*> Instance;

	};
}
