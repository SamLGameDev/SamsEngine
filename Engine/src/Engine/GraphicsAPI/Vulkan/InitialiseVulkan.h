
#pragma once

#include <string>
#include <vulkan/vulkan_core.h>

#include "Array.h"
#include "ErrorCodes.h"
#include <optional>


struct QueueFamilyIndices
{
	std::optional<uint32_t> GraphicsFamily;

	float QueuePriority = 1;

	bool IsComplete() const
	{
		return GraphicsFamily.has_value();
	}
};


inline VkResult CreateDebugMessageUtilsMessengerEXT(
	VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger)
{
	auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
	if (func != nullptr)
	{
		return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
	}
	return VK_ERROR_EXTENSION_NOT_PRESENT;
}

inline void DestroyDebugUtilsMessengerEXT(
	VkInstance instance, const VkDebugUtilsMessengerEXT DebugMessenger, const VkAllocationCallbacks* pAllocator)
{
	auto func = (PFN_vkDestroyDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
	if (func != nullptr)
	{
		return func(instance, DebugMessenger, pAllocator);
	}
}


class InitialiseVulkan
{
public:

	InitialiseVulkan() = default;

	ErrorCodes CreateVulkan();
	static void PopulateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);
	ErrorCodes CreateDebugMessenger();
	ErrorCodes FindGraphicsCard();
	ErrorCodes CreateLogicalDevice();
	ErrorCodes Init();

	ErrorCodes Shutdown();


private:

	VkInstance Instance;
	VkDebugUtilsMessengerEXT DebugMessenger;
	VkPhysicalDevice GraphicsCard = VK_NULL_HANDLE;
	VkDevice LogicalDevice;
	VkQueue GraphicsQueue;

	QueueFamilyIndices Indices;

	constexpr static uint32_t ApplicationVersion = VK_MAKE_VERSION(1, 0, 0);
	constexpr static uint32_t EngineVersion = VK_MAKE_VERSION(1, 0, 0);
	constexpr static uint32_t APIVersion = VK_API_VERSION_1_0;

	
	const static Array<const char*> ValidationLayers;

	bool CheckValidationLayerSupport();

	Array<const char*> GetRequiredExtensions();

	static VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback
	(
		VkDebugUtilsMessageSeverityFlagBitsEXT Serverity,
		VkDebugUtilsMessageTypeFlagsEXT Type,
		const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
		void* pUserData
	);

	static bool IsDeviceSuitable(const VkPhysicalDevice& Device);

	static QueueFamilyIndices FindQueueFamilies(const VkPhysicalDevice& Device);

};
