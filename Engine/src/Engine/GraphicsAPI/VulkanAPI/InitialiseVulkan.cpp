#include "InitialiseVulkan.h"

#include <iostream>
#include <cstdint>
#include <glad/glad.h>
#include "GLFW/glfw3.h"
#include "FirstWindow.h"
#include <set>
#include "Vector2D.h"
#include <fstream>
#include "FileSystem/CorePaths.h"
#include <sstream>
#include "VulkanInstance.h"

namespace Vulkan
{

#if DEBUG
const Array<const char*> InitialiseVulkan::ValidationLayers = { "VK_LAYER_KHRONOS_validation" };
#endif
#if RELEASE
const Array<const char*> InitialiseVulkan::ValidationLayers;
#endif

ErrorCodes InitialiseVulkan::CreateVulkan()
{
	glfwInit();

# if DEBUG
	if (!CheckValidationLayerSupport())
	{
		return ERROR;
	}
#endif

	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

	VkApplicationInfo appInfo{};

	appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	appInfo.pApplicationName = "Voronoi";
	appInfo.applicationVersion = ApplicationVersion;
	appInfo.pEngineName = "SamsEngine";
	appInfo.engineVersion = EngineVersion;
	appInfo.apiVersion = APIVersion;

	VkInstanceCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	createInfo.pApplicationInfo = &appInfo;

	Array<const char*> extensions = GetRequiredExtensions();

	createInfo.enabledExtensionCount = extensions.GetSize();
	createInfo.ppEnabledExtensionNames = extensions.GetFirstPtr();
	createInfo.enabledLayerCount = static_cast<std::uint32_t>(ValidationLayers.GetSize());
	createInfo.ppEnabledLayerNames = ValidationLayers.GetFirstPtr();
	createInfo.flags = 0;

	std::uint32_t optionalExtensionsCount;

	vkEnumerateInstanceExtensionProperties(nullptr, &optionalExtensionsCount, nullptr);

	Array<VkExtensionProperties> OptionalExtensions(optionalExtensionsCount);

	vkEnumerateInstanceExtensionProperties(nullptr, &optionalExtensionsCount, OptionalExtensions.GetFirstPtr());

	for (size_t i = 0; i < extensions.GetSize(); i++) {

		bool bContains = false;

		for (size_t j = 0; j < optionalExtensionsCount; j++)
		{
			std::string name = extensions[i];

			if (name == OptionalExtensions[j].extensionName)
			{
				bContains = true;
				break;
			}
		}

		if (!bContains)
		{
			std::cout << extensions[i] << " Is not Supported \n";
		}
	}

#if DEBUG

	VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo;

	PopulateDebugMessengerCreateInfo(debugCreateInfo);

	createInfo.pNext = &debugCreateInfo;

#endif


	VkResult result = vkCreateInstance(&createInfo, nullptr, &SInstance::GetInstance()->VulkanInstance);

	if (result != VK_SUCCESS)
	{
		std::cout << "ERROR::VULKAN::FAILED TO INITIALISE VULKAN";
		return ERROR;
	}

	return SUCCEEDED;
}

void InitialiseVulkan::PopulateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo)
{
	createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
	createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
	createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT;
	createInfo.pfnUserCallback = DebugCallback;
	createInfo.pUserData = nullptr;
	createInfo.flags = 0;
}

ErrorCodes InitialiseVulkan::CreateDebugMessenger()
{
	VkDebugUtilsMessengerCreateInfoEXT createInfo;

	PopulateDebugMessengerCreateInfo(createInfo);

	if (CreateDebugMessageUtilsMessengerEXT(Instance, &createInfo, nullptr, &DebugMessenger) != VK_SUCCESS)
	{
		return ERROR;
	}
	return SUCCEEDED;
}

ErrorCodes InitialiseVulkan::Init()
{
	ErrorCodes success = CreateVulkan();
	if (success == ERROR)
	{
		return ERROR;
	}

#if DEBUG
	if (CreateDebugMessenger() == ERROR) return ERROR;

#endif


	return SUCCEEDED;
}


ErrorCodes InitialiseVulkan::Shutdown()
{

#if DEBUG
	DestroyDebugUtilsMessengerEXT(Instance, DebugMessenger, nullptr);
#endif


	vkDestroyInstance(Instance, nullptr);

	return SUCCEEDED;
}

bool InitialiseVulkan::CheckValidationLayerSupport()
{
	std::uint32_t layerCount;

	vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

	Array<VkLayerProperties> availableLayers(layerCount);

	vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.GetFirstPtr());

	for (const char* layer : ValidationLayers)
	{
		bool bContains = false;
		for (size_t i = 0; i < layerCount; i++)
		{
			if (std::strcmp(availableLayers[i].layerName, layer) == 0)
			{
				bContains = true;
				break;
			}
		}

		if (!bContains)
		{
			std::cout << "ERROR::VULKAN::VALIDATION LAYER NOT AVAILABLE \n";
			return false;
		}
	}

	return true;
}

Array<const char*> InitialiseVulkan::GetRequiredExtensions()
{
	uint32_t ExtensionsCount;
	const char** glfwExtensions = glfwGetRequiredInstanceExtensions(&ExtensionsCount);

	Array<const char*> extensions(glfwExtensions, glfwExtensions + ExtensionsCount);

#if DEBUG
	extensions.Add(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
#endif


	return extensions;
}

VkBool32 InitialiseVulkan::DebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT Serverity,
	VkDebugUtilsMessageTypeFlagsEXT Type, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData)
{
	std::cerr << "ERROR::VULKAN::VALIDATION ERROR::" << pCallbackData->pMessage << "\n";

	return VK_FALSE;
}
}
