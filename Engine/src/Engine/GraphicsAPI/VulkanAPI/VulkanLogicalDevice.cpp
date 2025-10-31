#include "VulkanLogicalDevice.h"
#include <iostream>
#include <set>

#include "VulkanInstance.h"

#include "VulkanGraphicsCard.h"

namespace Vulkan
{

	ULogicalDevice::ULogicalDevice(UGraphicsCard* InOwningCard)
	{
		OwningCard = InOwningCard;
		Init();
		SwapChain = new USwapChain(this);
	}

	ULogicalDevice::~ULogicalDevice()
	{
		ShutDown();
	}

	ErrorCodes ULogicalDevice::Init()
	{
		if (CreateLogicalDevice() == ERROR)
		{
			return ERROR;
		}
		return SUCCEEDED;
	}

	ErrorCodes ULogicalDevice::CreateLogicalDevice()
	{

		const QueueFamilyIndices& indices = OwningCard->GetFoundQueueFamilies();

		std::set<std::uint32_t> queues = {indices.GraphicsFamily.value(), indices.PresentFamily.value() };

		Array<VkDeviceQueueCreateInfo> uniqueQueues;

		for (uint32_t queue : queues)
		{
			VkDeviceQueueCreateInfo createInfo{};
			createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
			createInfo.queueFamilyIndex = queue;
			createInfo.queueCount = 1;
			createInfo.pQueuePriorities = &indices.QueuePriority;
			uniqueQueues.Add(createInfo);
		}


		VkPhysicalDeviceFeatures deviceFeatures{};

		const Array<const char*>& deviceExtensions = OwningCard->GetDeviceExtensions();

		VkDeviceCreateInfo deviceCreateInfo{};
		deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
		deviceCreateInfo.pQueueCreateInfos = uniqueQueues.GetFirstRef();
		deviceCreateInfo.queueCreateInfoCount = uniqueQueues.GetSize();

		deviceCreateInfo.pEnabledFeatures = &deviceFeatures;
		deviceCreateInfo.enabledExtensionCount = deviceExtensions.GetSize();
		deviceCreateInfo.ppEnabledExtensionNames = deviceExtensions.GetFirstRef();

		deviceCreateInfo.enabledLayerCount = 0;

#if DEBUG

		const Array<const char*>& validationLayers = OwningCard->GetValidationLayers();


		deviceCreateInfo.enabledLayerCount = validationLayers.GetSize();
		deviceCreateInfo.ppEnabledLayerNames = validationLayers.GetFirstRef();
#endif

		if (vkCreateDevice(*OwningCard->GetVulkanPhysicalDevice(), &deviceCreateInfo, nullptr, &LogicalDevice) != VK_SUCCESS)
		{
			std::cerr << "ERROR::VULKAN::FAILED TO CREATE LOGICAL DEVICE";
			return ERROR;
		}

		vkGetDeviceQueue(LogicalDevice, indices.GraphicsFamily.value(), 0, &GraphicsQueue);
		vkGetDeviceQueue(LogicalDevice, indices.PresentFamily.value(), 0, &PresentQueue);
		return SUCCEEDED;
	}

	ErrorCodes ULogicalDevice::ShutDown()
	{
		vkDestroyDevice(LogicalDevice, nullptr);
		return SUCCEEDED;
	}
}
