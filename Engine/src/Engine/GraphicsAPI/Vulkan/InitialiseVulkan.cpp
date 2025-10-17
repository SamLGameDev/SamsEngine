#include "InitialiseVulkan.h"

#include <iostream>
#include <cstdint>

#include "GLFW/glfw3.h"

#if DEBUG
const Array<const char*> InitialiseVulkan::ValidationLayers = { "VK_LAYER_KHRONOS_validation" };
#endif
#if RELEASE
const Array<const char*> InitialiseVulkan::ValidationLayers;
#endif


ErrorCodes InitialiseVulkan::CreateVulkan()
{
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
	createInfo.ppEnabledExtensionNames = extensions.GetFirstRef();
	createInfo.enabledLayerCount = static_cast<std::uint32_t>(ValidationLayers.GetSize());
	createInfo.ppEnabledLayerNames = ValidationLayers.GetFirstRef();
	createInfo.flags = 0;

	std::uint32_t optionalExtensionsCount;

	vkEnumerateInstanceExtensionProperties(nullptr, &optionalExtensionsCount, nullptr);

	Array<VkExtensionProperties> OptionalExtensions(optionalExtensionsCount);

	vkEnumerateInstanceExtensionProperties(nullptr, &optionalExtensionsCount, OptionalExtensions.GetFirstRef());


	//std::cout << "Number of extensions: " << optionalExtensionsCount << " available extensions:\n";

	//for (auto extension : OptionalExtensions)
	//{
	//	std::cout << extension.extensionName << "\n";
	//}

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


	VkResult result = vkCreateInstance(&createInfo, nullptr, &Instance);

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

ErrorCodes InitialiseVulkan::FindGraphicsCard()
{
	std::uint32_t NumDevices;
	vkEnumeratePhysicalDevices(Instance, &NumDevices, nullptr);

	if (NumDevices == 0)
	{
		std::cout << "ERROR::VULKAN::NO PHYSICAL DEVICES FOUND";
		return ERROR;
	}

	Array<VkPhysicalDevice> physicalDevices(NumDevices);
	vkEnumeratePhysicalDevices(Instance, &NumDevices, physicalDevices.GetFirstRef());

	for (const VkPhysicalDevice& device : physicalDevices)
	{
		if (IsDeviceSuitable(device))
		{
			GraphicsCard = device;
			Indices = FindQueueFamilies(GraphicsCard);
			return SUCCEEDED;
		}
	}
	std::cerr << "ERROR::VULKAN::GPU::NO SUITABLE GRAPHICS CARD";
	return ERROR;
}

ErrorCodes InitialiseVulkan::CreateLogicalDevice()
{
	VkDeviceQueueCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
	createInfo.queueFamilyIndex = Indices.GraphicsFamily.value();
	createInfo.queueCount = 1;

	createInfo.pQueuePriorities = &Indices.QueuePriority;

	VkPhysicalDeviceFeatures deviceFeatures{};

	VkDeviceCreateInfo deviceCreateInfo{};
	deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	deviceCreateInfo.pQueueCreateInfos = &createInfo;
	deviceCreateInfo.queueCreateInfoCount = 1;

	deviceCreateInfo.pEnabledFeatures = &deviceFeatures;
	deviceCreateInfo.enabledExtensionCount = 0;

	deviceCreateInfo.enabledLayerCount = 0;

#if DEBUG
	deviceCreateInfo.enabledLayerCount = ValidationLayers.GetSize();
	deviceCreateInfo.ppEnabledLayerNames = ValidationLayers.GetFirstRef();
#endif

	if (vkCreateDevice(GraphicsCard, &deviceCreateInfo, nullptr, &LogicalDevice) != VK_SUCCESS)
	{
		std::cerr << "ERROR::VULKAN::FAILED TO CREATE LOGICAL DEVICE";
		return ERROR;
	}

	vkGetDeviceQueue(LogicalDevice, Indices.GraphicsFamily.value(), 0, &GraphicsQueue);
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

	//if (glfwCreateWindowSurface(Instance, , nullptr, WindowsInterface) != VK_SUCCESS)
	//{
	//	return ERROR;
	//}

#endif

	if (FindGraphicsCard() == ERROR) return ERROR;

	if (CreateLogicalDevice() == ERROR) return ERROR;

	return SUCCEEDED;
}

ErrorCodes InitialiseVulkan::Shutdown()
{
	vkDestroyInstance(Instance, nullptr);

#if DEBUG
	DestroyDebugUtilsMessengerEXT(Instance, DebugMessenger, nullptr);
#endif

	vkDestroyDevice(LogicalDevice, nullptr);


	return SUCCEEDED;
}

bool InitialiseVulkan::CheckValidationLayerSupport()
{
	std::uint32_t layerCount;

	vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

	Array<VkLayerProperties> availableLayers(layerCount);

	vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.GetFirstRef());

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

bool InitialiseVulkan::IsDeviceSuitable(const VkPhysicalDevice& Device)
{

	VkPhysicalDeviceProperties deviceProperties;
	vkGetPhysicalDeviceProperties(Device, &deviceProperties);

	VkPhysicalDeviceFeatures deviceFeatures;
	vkGetPhysicalDeviceFeatures(Device, &deviceFeatures);

    bool suitable = deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU &&
		deviceFeatures.fragmentStoresAndAtomics &&
		deviceFeatures.geometryShader &&
		FindQueueFamilies(Device).IsComplete();

#if DEBUG
	if (suitable)
	{
		std::cout << deviceProperties.deviceName << "\n";
	}
#endif


	return suitable;
}

QueueFamilyIndices InitialiseVulkan::FindQueueFamilies(const VkPhysicalDevice& Device)
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

		if (indices.IsComplete()) break;
	}

	return indices;
}
