
#pragma once

#include <string>


#include "Array.h"
#include "ErrorCodes.h"
#include <optional>
#include "vulkan/vulkan.h"

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
	void AttachToWindow();
	ErrorCodes Init();

	ErrorCodes Shutdown();


private:

	VkInstance Instance;
	VkDebugUtilsMessengerEXT DebugMessenger;
	VkPhysicalDevice GraphicsCard = VK_NULL_HANDLE;
	VkDevice LogicalDevice;
	VkQueue GraphicsQueue;
	VkQueue PresentQueue;
	VkSurfaceKHR WindowsInterface;
	SwapChainSupportDetails SwapChainSupport;
	VkSwapchainKHR SwapChain = VK_NULL_HANDLE;

	Array<VkImage> SwapChainImages;

	Array<VkImageView> SwapChainImageViews;

	VkSurfaceFormatKHR SwapChainFomat;
	VkExtent2D SwapChainExtent;


	QueueFamilyIndices Indices;

	constexpr static uint32_t ApplicationVersion = VK_MAKE_VERSION(1, 0, 0);
	constexpr static uint32_t EngineVersion = VK_MAKE_VERSION(1, 0, 0);
	constexpr static uint32_t APIVersion = VK_API_VERSION_1_0;

	
	const static Array<const char*> ValidationLayers;

	const static Array<const char*> DeviceExtensions;

	bool CheckValidationLayerSupport();

	Array<const char*> GetRequiredExtensions();

	static VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback
	(
		VkDebugUtilsMessageSeverityFlagBitsEXT Serverity,
		VkDebugUtilsMessageTypeFlagsEXT Type,
		const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
		void* pUserData
	);
	static bool DoesDeviceHaveRequiredExtensions(const VkPhysicalDevice& Device);
	bool IsSwapChainSupported(const VkPhysicalDevice& Device, const VkSurfaceKHR& Surface);

	bool IsDeviceSuitable(const VkPhysicalDevice& Device, const VkSurfaceKHR& Surface);

	static QueueFamilyIndices FindQueueFamilies(const VkPhysicalDevice& Device, const VkSurfaceKHR& Surface);

	static VkSurfaceFormatKHR ChooseSwapChainFormat(const Array<VkSurfaceFormatKHR>& AvailableFormats);
	static VkPresentModeKHR ChooseSwapChainPresent(const Array<VkPresentModeKHR>& AvailablePresents);
	static VkExtent2D ChooseSwapChainExtent(const VkSurfaceCapabilitiesKHR& capabilities);

	ErrorCodes CreateSwapChain();


	ErrorCodes CreateImageViews();

};
