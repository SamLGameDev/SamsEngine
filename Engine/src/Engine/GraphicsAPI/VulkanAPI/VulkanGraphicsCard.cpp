#include "VulkanGraphicsCard.h"
#include <iostream>
#include "VulkanInstance.h"
#include "VulkanLogicalDevice.h"

namespace Vulkan
{

	ErrorCodes UGraphicsCard::Init()
	{
		if (FindGraphicsCard() == ERROR) return ERROR;

		LogicalDevice = new ULogicalDevice(this);

		CreateRenderer();

		LogicalDevice->GetSwapChain()->CreateFrameBuffers();

		return SUCCEEDED;
	}

	ErrorCodes UGraphicsCard::FindGraphicsCard()
	{
		std::uint32_t NumDevices;
		vkEnumeratePhysicalDevices(SInstance::GetInstance()->VulkanInstance, &NumDevices, nullptr);

		if (NumDevices == 0)
		{
			std::cout << "ERROR::VULKAN::NO PHYSICAL DEVICES FOUND";
			return ERROR;
		}

		Array<VkPhysicalDevice> physicalDevices(NumDevices);
		vkEnumeratePhysicalDevices(SInstance::GetInstance()->VulkanInstance, &NumDevices, physicalDevices.GetFirstRef());

		for (const VkPhysicalDevice& device : physicalDevices)
		{
			if (IsDeviceSuitable(device, SInstance::GetInstance()->WindowsInterface))
			{
				GraphicsCard = device;
				Indices = FindQueueFamilies(GraphicsCard, SInstance::GetInstance()->WindowsInterface);
				return SUCCEEDED;
			}
		}
		std::cerr << "ERROR::VULKAN::GPU::NO SUITABLE GRAPHICS CARD";
		return ERROR;
	}

	bool UGraphicsCard::IsDeviceSuitable(const VkPhysicalDevice& Device, const VkSurfaceKHR& Surface)
	{

		vkGetPhysicalDeviceProperties(Device, &Properties);

		VkPhysicalDeviceFeatures deviceFeatures;
		vkGetPhysicalDeviceFeatures(Device, &deviceFeatures);

		const bool bHasRequiredExtensions = DoesDeviceHaveRequiredExtensions(Device);

		const bool bSwapChainSupported = IsSwapChainSupported(Device, Surface);


		bool suitable = Properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU &&
			deviceFeatures.fragmentStoresAndAtomics &&
			deviceFeatures.geometryShader &&
			deviceFeatures.samplerAnisotropy &&
			FindQueueFamilies(Device, Surface).IsComplete() && bHasRequiredExtensions && bSwapChainSupported;

#if DEBUG
		if (suitable)
		{
			std::cout << Properties.deviceName << "\n";
		}
#endif


		return suitable;
	}

	bool UGraphicsCard::DoesDeviceHaveRequiredExtensions(const VkPhysicalDevice& Device)
	{
		uint32_t extensionCount;
		vkEnumerateDeviceExtensionProperties(Device, nullptr, &extensionCount, nullptr);

		Array<VkExtensionProperties> extensions(extensionCount);
		vkEnumerateDeviceExtensionProperties(Device, nullptr, &extensionCount, extensions.GetFirstRef());

		for (const char* requiredExtension : DeviceExtensions)
		{
			bool contains = false;
			for (VkExtensionProperties extension : extensions)
			{
				if (std::strcmp(extension.extensionName, requiredExtension) == 0)
				{
					contains = true;
					break;
				}
			}
			if (!contains)
			{
				return false;
			}

		}
		return true;
	}

	bool UGraphicsCard::IsSwapChainSupported(const VkPhysicalDevice& Device, const VkSurfaceKHR& Surface)
	{
		vkGetPhysicalDeviceSurfaceCapabilitiesKHR(Device, Surface, &SwapChainSupport.Capabilities);

		std::uint32_t formatCount;
		vkGetPhysicalDeviceSurfaceFormatsKHR(Device, Surface, &formatCount, nullptr);

		if (formatCount != 0)
		{
			SwapChainSupport.Formats.Reallocate(formatCount);
			vkGetPhysicalDeviceSurfaceFormatsKHR(Device, Surface, &formatCount, SwapChainSupport.Formats.GetFirstRef());
		}

		std::uint32_t presentsModeCount;
		vkGetPhysicalDeviceSurfacePresentModesKHR(Device, Surface, &presentsModeCount, nullptr);

		if (presentsModeCount != 0)
		{
			SwapChainSupport.Presents.Reallocate(presentsModeCount);
			vkGetPhysicalDeviceSurfacePresentModesKHR(Device, Surface, &presentsModeCount, SwapChainSupport.Presents.GetFirstRef());
		}

		return !SwapChainSupport.Presents.IsEmpty() && !SwapChainSupport.Formats.IsEmpty();
	}

	uint32_t UGraphicsCard::FindMemoryType(uint32_t TypeFilter, VkMemoryPropertyFlags Properties)
	{
		VkPhysicalDeviceMemoryProperties properties;
		vkGetPhysicalDeviceMemoryProperties(GraphicsCard, &properties);

		for (size_t i = 0; i < properties.memoryTypeCount; i++)
		{
			if (TypeFilter & (1 << i) && (properties.memoryTypes[i].propertyFlags & Properties) == Properties)
			{
				return i;
			}
		}
		throw std::runtime_error("Cant find a suitable memory type");
	}

	QueueFamilyIndices UGraphicsCard::FindQueueFamilies(const VkPhysicalDevice& Device, const VkSurfaceKHR& Surface)
	{
		QueueFamilyIndices indices;

		std::uint32_t queueFamilyCount;
		vkGetPhysicalDeviceQueueFamilyProperties(Device, &queueFamilyCount, nullptr);

		Array<VkQueueFamilyProperties> queueFamily(queueFamilyCount);
		vkGetPhysicalDeviceQueueFamilyProperties(Device, &queueFamilyCount, queueFamily.GetFirstRef());

		for (size_t i = 0; i < queueFamily.GetSize(); i++)
		{
			if (queueFamily[i].queueFlags & VK_QUEUE_GRAPHICS_BIT)
			{
				indices.GraphicsFamily = i;
			}

			VkBool32 bHasSurfaceSupport = false;
			vkGetPhysicalDeviceSurfaceSupportKHR(Device, i, Surface, &bHasSurfaceSupport);

			if (bHasSurfaceSupport)
			{
				indices.PresentFamily = i;
			}

			if (indices.IsComplete()) break;
		}

		return indices;
	}

	VkFormat UGraphicsCard::FindSupportedFormat(const Array<VkFormat>& Candidates, const VkImageTiling& Tilling,
		const VkFormatFeatureFlags& Features) const
	{
		for (const auto& format : Candidates)
		{
			VkFormatProperties props;
			vkGetPhysicalDeviceFormatProperties(GraphicsCard, format, &props);

			if (Tilling == VK_IMAGE_TILING_LINEAR && (props.linearTilingFeatures & Features) == Features)
			{
				return format;
			}

			if (Tilling == VK_IMAGE_TILING_OPTIMAL && (props.optimalTilingFeatures & Features) == Features)
			{
				return format;
			}
		}
		throw std::runtime_error("Failed to find Supported format");
	}

	VkFormat UGraphicsCard::FindDepthFormat() const
	{
		return FindSupportedFormat({ VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT, VK_FORMAT_D32_SFLOAT },
			VK_IMAGE_TILING_OPTIMAL, VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT);
	}

	bool UGraphicsCard::HadStencilAttachment(const VkFormat& Format) const
	{
		return Format == VK_FORMAT_D32_SFLOAT_S8_UINT || Format == VK_FORMAT_D24_UNORM_S8_UINT || Format == VK_FORMAT_D16_UNORM_S8_UINT;
	}


	void UGraphicsCard::CreateRenderer()
	{

		Renderer = new URenderer(this);
		Renderer->Init();
	}

	ErrorCodes UGraphicsCard::ShutDown()
	{
		delete Renderer;

		delete LogicalDevice;

		return SUCCEEDED;
	
	}

	UGraphicsCard::UGraphicsCard()
	{
	}
}
