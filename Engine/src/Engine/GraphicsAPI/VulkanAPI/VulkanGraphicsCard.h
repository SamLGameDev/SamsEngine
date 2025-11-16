
#pragma once

#include "ErrorCodes.h"
#include "GraphicsAPIConstructor.h"
#include "VulkanRenderer.h"

namespace Vulkan
{
	class ULogicalDevice;
}

namespace Vulkan
{

	struct SwapChainSupportDetails
	{
		VkSurfaceCapabilitiesKHR Capabilities;
		Array<VkSurfaceFormatKHR> Formats;
		Array<VkPresentModeKHR> Presents;
	};

	struct QueueFamilyIndices
	{
		std::optional<uint32_t> GraphicsFamily;
		std::optional<uint32_t> PresentFamily;

		float QueuePriority = 1;

		bool IsComplete() const
		{
			return GraphicsFamily.has_value() && PresentFamily.has_value();
		}
	};

	class UGraphicsCard
	{
	public:

		UGraphicsCard();

		ErrorCodes Init();
		ErrorCodes FindGraphicsCard();
		bool IsDeviceSuitable(const VkPhysicalDevice& Device, const VkSurfaceKHR& Surface);
		bool DoesDeviceHaveRequiredExtensions(const VkPhysicalDevice& Device);
		bool IsSwapChainSupported(const VkPhysicalDevice& Device, const VkSurfaceKHR& Surface);

		uint32_t FindMemoryType(uint32_t TypeFilter, VkMemoryPropertyFlags Properties);


		QueueFamilyIndices FindQueueFamilies(const VkPhysicalDevice& Device, const VkSurfaceKHR& Surface);

		VkFormat FindSupportedFormat(const Array<VkFormat>& Candidates, const VkImageTiling& Tilling, const VkFormatFeatureFlags& Features) const;

		VkFormat FindDepthFormat() const;

		bool HadStencilAttachment(const VkFormat& Format) const;

		const QueueFamilyIndices& GetFoundQueueFamilies() const
		{
			return Indices;
		}

		const Array<const char*>& GetDeviceExtensions() const
		{
			return DeviceExtensions;
		}

		const Array<const char*>& GetValidationLayers() const
		{
			return ValidationLayers;
		}

		VkPhysicalDevice* GetVulkanPhysicalDevice() 
		{
			return &GraphicsCard;
		}

		const SwapChainSupportDetails& GetSwapChainSupportDetails() const
		{
			return SwapChainSupport;
		}

		const ULogicalDevice* GetLogicalDevice() const
		{
			return LogicalDevice;
		}

		URenderer* GetRenderer()
		{
			return Renderer;
		};

		VkPhysicalDeviceProperties GetDeviceProperties() const
		{
			return Properties;
		}

		void CreateRenderer();

		ErrorCodes ShutDown();

	private:

		VkPhysicalDevice GraphicsCard;

		ULogicalDevice* LogicalDevice;

		Array<const char*> DeviceExtensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME, VK_EXT_VERTEX_INPUT_DYNAMIC_STATE_EXTENSION_NAME };

		SwapChainSupportDetails SwapChainSupport;

		QueueFamilyIndices Indices;

		URenderer* Renderer;

		VkPhysicalDeviceProperties Properties;

		const Array<const char*> ValidationLayers = { "VK_LAYER_KHRONOS_validation" };
//#if DEBUG
//		const Array<const char*> ValidationLayers = { "VK_LAYER_KHRONOS_validation" };
//#endif
//#if RELEASE
//		const Array<const char*> ValidationLayers;
//#endif
//#ifndef ValidationLayers
//		const Array<const char*> ValidationLayers;
//#endif
	};
}
