// DO NOT MARK
//This is because it has been submitted for my COMP305. Link to Original: https://github.falmouth.ac.uk/GA-Undergrad-Student-Work-25-26/Comp305-Engine-SL295211.git
#include "VulkanLogicalDevice.h"
#include <iostream>
#include <set>

#include "VulkanInstance.h"

#include "VulkanGraphicsCard.h"

namespace Vulkan
{
	void ULogicalDevice::CreateSwapChain()
	{
		SwapChain = new USwapChain(this);
	}

	ULogicalDevice::ULogicalDevice(UGraphicsCard* InOwningCard)
	{
		OwningCard = InOwningCard;
		Init();
		CreateSwapChain();
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
		deviceFeatures.geometryShader = VK_TRUE;
		deviceFeatures.samplerAnisotropy = VK_TRUE;
		deviceFeatures.shaderFloat64 = VK_TRUE;

		const Array<const char*>& deviceExtensions = OwningCard->GetDeviceExtensions();

		VkPhysicalDeviceVertexInputDynamicStateFeaturesEXT vertexInputDynamicStateFeatures{};
		vertexInputDynamicStateFeatures.sType =
			VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VERTEX_INPUT_DYNAMIC_STATE_FEATURES_EXT;
		vertexInputDynamicStateFeatures.vertexInputDynamicState = VK_TRUE;

		VkDeviceCreateInfo deviceCreateInfo{};
		deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
		deviceCreateInfo.pQueueCreateInfos = uniqueQueues.GetFirstPtr();
		deviceCreateInfo.queueCreateInfoCount = uniqueQueues.GetSize();
		deviceCreateInfo.pNext = &vertexInputDynamicStateFeatures;
		deviceCreateInfo.pEnabledFeatures = &deviceFeatures;
		deviceCreateInfo.enabledExtensionCount = deviceExtensions.GetSize();
		deviceCreateInfo.ppEnabledExtensionNames = deviceExtensions.GetFirstPtr();

		deviceCreateInfo.enabledLayerCount = 0;

#if DEBUG

		const Array<const char*>& validationLayers = OwningCard->GetValidationLayers();


		deviceCreateInfo.enabledLayerCount = validationLayers.GetSize();
		deviceCreateInfo.ppEnabledLayerNames = validationLayers.GetFirstPtr();
#endif


		if (vkCreateDevice(*OwningCard->GetVulkanPhysicalDevice(), &deviceCreateInfo, nullptr, &LogicalDevice) != VK_SUCCESS)
		{
			std::cerr << "ERROR::VULKAN::FAILED TO CREATE LOGICAL DEVICE";
			return ERROR;
		}

		vkGetDeviceQueue(LogicalDevice, indices.GraphicsFamily.value(), 0, &GraphicsQueue);
		vkGetDeviceQueue(LogicalDevice, indices.PresentFamily.value(), 0, &PresentQueue);
		vkGetDeviceQueue(LogicalDevice, indices.GraphicsAndComputeFamily.value(), 0, &ComputeQueue);
		return SUCCEEDED;
	}

	ErrorCodes ULogicalDevice::ShutDown()
	{
		SwapChain->ShutDown();
		vkDestroyDevice(LogicalDevice, nullptr);
		return SUCCEEDED;
	}
}
