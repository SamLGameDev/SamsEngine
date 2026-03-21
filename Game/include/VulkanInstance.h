// DO NOT MARK except ActiveComputeShader
//This is because it has been submitted for my COMP305. Link to Original: https://github.falmouth.ac.uk/GA-Undergrad-Student-Work-25-26/Comp305-Engine-SL295211.git
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
