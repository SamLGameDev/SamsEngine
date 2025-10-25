#include "InitialiseVulkan.h"

#include <iostream>
#include <cstdint>
#include <glad/glad.h>
#include "GLFW/glfw3.h"
#include "FirstWindow.h"
#include <set>
#include "Vector2D.h"

#if DEBUG
const Array<const char*> InitialiseVulkan::ValidationLayers = { "VK_LAYER_KHRONOS_validation" };
#endif
#if RELEASE
const Array<const char*> InitialiseVulkan::ValidationLayers;
#endif

const Array<const char*> InitialiseVulkan::DeviceExtensions = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};

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
		if (IsDeviceSuitable(device, WindowsInterface))
		{
			GraphicsCard = device;
			Indices = FindQueueFamilies(GraphicsCard, WindowsInterface);
			return SUCCEEDED;
		}
	}
	std::cerr << "ERROR::VULKAN::GPU::NO SUITABLE GRAPHICS CARD";
	return ERROR;
}

ErrorCodes InitialiseVulkan::CreateLogicalDevice()
{

	std::set<std::uint32_t> queues = {Indices.GraphicsFamily.value(), Indices.PresentFamily.value()};

	Array<VkDeviceQueueCreateInfo> uniqueQueues;

	for (uint32_t queue : queues)
	{
		VkDeviceQueueCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		createInfo.queueFamilyIndex = queue;
		createInfo.queueCount = 1;
		createInfo.pQueuePriorities = &Indices.QueuePriority;
		uniqueQueues.Add(createInfo);
	}


	VkPhysicalDeviceFeatures deviceFeatures{};

	VkDeviceCreateInfo deviceCreateInfo{};
	deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	deviceCreateInfo.pQueueCreateInfos = uniqueQueues.GetFirstRef();
	deviceCreateInfo.queueCreateInfoCount = uniqueQueues.GetSize();

	deviceCreateInfo.pEnabledFeatures = &deviceFeatures;
	deviceCreateInfo.enabledExtensionCount = DeviceExtensions.GetSize();
	deviceCreateInfo.ppEnabledExtensionNames = DeviceExtensions.GetFirstRef();

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
	vkGetDeviceQueue(LogicalDevice, Indices.PresentFamily.value(), 0, &PresentQueue);
	return SUCCEEDED;
}

void InitialiseVulkan::AttachToWindow()
{
	if (glfwCreateWindowSurface(Instance, FirstWindow::Window, nullptr, &WindowsInterface) != VK_SUCCESS)
	{
		return;
	}

	if (FindGraphicsCard() == ERROR)
	{
		return;
	}

	if (CreateLogicalDevice() == ERROR)
	{
		return;
	}

	if (CreateSwapChain() == ERROR)
	{
		return;
	}

	if (CreateImageViews() == ERROR) return;


	while (!glfwWindowShouldClose(FirstWindow::Window))
	{
		glfwPollEvents();
	}

	Shutdown();
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
	for (VkImageView& view : SwapChainImageViews)
	{
		vkDestroyImageView(LogicalDevice, view, nullptr);
	}

	vkDestroySwapchainKHR(LogicalDevice, SwapChain, nullptr);

	vkDestroySurfaceKHR(Instance, WindowsInterface, nullptr);

	vkDestroyDevice(LogicalDevice, nullptr);

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

bool InitialiseVulkan::DoesDeviceHaveRequiredExtensions(const VkPhysicalDevice& Device)
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

bool InitialiseVulkan::IsSwapChainSupported(const VkPhysicalDevice& Device, const VkSurfaceKHR& Surface)
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

bool InitialiseVulkan::IsDeviceSuitable(const VkPhysicalDevice& Device, const VkSurfaceKHR& Surface)
{

	VkPhysicalDeviceProperties deviceProperties;
	vkGetPhysicalDeviceProperties(Device, &deviceProperties);

	VkPhysicalDeviceFeatures deviceFeatures;
	vkGetPhysicalDeviceFeatures(Device, &deviceFeatures);

	const bool bHasRequiredExtensions = DoesDeviceHaveRequiredExtensions(Device);

	const bool bSwapChainSupported = IsSwapChainSupported(Device, Surface);


	bool suitable = deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU &&
		deviceFeatures.fragmentStoresAndAtomics &&
		deviceFeatures.geometryShader &&
		FindQueueFamilies(Device, Surface).IsComplete() && bHasRequiredExtensions && bSwapChainSupported;

#if DEBUG
	if (suitable)
	{
		std::cout << deviceProperties.deviceName << "\n";
	}
#endif


	return suitable;
}

QueueFamilyIndices InitialiseVulkan::FindQueueFamilies(const VkPhysicalDevice& Device, const VkSurfaceKHR& Surface)
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

VkSurfaceFormatKHR InitialiseVulkan::ChooseSwapChainFormat(const Array<VkSurfaceFormatKHR>& AvailableFormats)
{
	for (const auto& format : AvailableFormats)
	{
		if (format.format == VK_FORMAT_R8G8B8A8_SRGB && format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
		{
			return format;
		}
	}

	return AvailableFormats[0];
}

VkPresentModeKHR InitialiseVulkan::ChooseSwapChainPresent(const Array<VkPresentModeKHR>& AvailablePresents)
{
	for (const auto& present : AvailablePresents)
	{
		if (present == VK_PRESENT_MODE_MAILBOX_KHR)
		{
			return present;
		}
	}

	return VK_PRESENT_MODE_FIFO_KHR;
}

VkExtent2D InitialiseVulkan::ChooseSwapChainExtent(const VkSurfaceCapabilitiesKHR& capabilities)
{
	if (capabilities.currentExtent.width != std::numeric_limits<std::uint32_t>::max()) return capabilities.currentExtent;

	std::int32_t x, y;
	glfwGetFramebufferSize(FirstWindow::Window, &x, &y);

	Vector2D actual = {static_cast<float>(x), static_cast<float>(y)};

	Vector2D min = {capabilities.minImageExtent.width, capabilities.minImageExtent.height};

	Vector2D max = { capabilities.maxImageExtent.width, capabilities.maxImageExtent.height };

	Vector2D::Clamp(actual, min, max);

	return { static_cast<std::uint32_t>(actual.X), static_cast<std::uint32_t>(actual.Y) };



}

ErrorCodes InitialiseVulkan::CreateSwapChain()
{
	VkSurfaceFormatKHR format = ChooseSwapChainFormat(SwapChainSupport.Formats);
	VkPresentModeKHR present = ChooseSwapChainPresent(SwapChainSupport.Presents);
	VkExtent2D extent = ChooseSwapChainExtent(SwapChainSupport.Capabilities);

	std::uint32_t imageCount = SwapChainSupport.Capabilities.minImageCount + 1;

	if (SwapChainSupport.Capabilities.maxImageCount > 0 && imageCount > SwapChainSupport.Capabilities.maxImageCount)
	{
		imageCount = SwapChainSupport.Capabilities.maxImageCount;
	}

	VkSwapchainCreateInfoKHR createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	createInfo.imageExtent = extent;
	createInfo.imageFormat = format.format;
	createInfo.presentMode = present;
	createInfo.imageColorSpace = format.colorSpace;
	createInfo.surface = WindowsInterface;
	createInfo.minImageCount = imageCount;
	createInfo.imageArrayLayers = 1;
	createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

	const Array<std::uint32_t> indices = {Indices.GraphicsFamily.value(), Indices.PresentFamily.value()};

	if (Indices.GraphicsFamily != Indices.PresentFamily)
	{
		createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
		createInfo.queueFamilyIndexCount = indices.GetSize();
		createInfo.pQueueFamilyIndices = indices.GetFirstRef();
	}
	else
	{
		createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
		createInfo.queueFamilyIndexCount = 0;
		createInfo.pQueueFamilyIndices = nullptr;
	}

	createInfo.preTransform = SwapChainSupport.Capabilities.currentTransform;
	createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	createInfo.clipped = VK_TRUE;
	createInfo.oldSwapchain = VK_NULL_HANDLE;

	if (vkCreateSwapchainKHR(LogicalDevice, &createInfo, nullptr, &SwapChain) != VK_SUCCESS) return ERROR;

	uint32_t swapChainImageCount;

	vkGetSwapchainImagesKHR(LogicalDevice, SwapChain, &swapChainImageCount, nullptr);

	SwapChainImages.Reallocate(swapChainImageCount);

	vkGetSwapchainImagesKHR(LogicalDevice, SwapChain, &swapChainImageCount, SwapChainImages.GetFirstRef());

	SwapChainExtent = extent;
	SwapChainFomat = format;


	return SUCCEEDED;
}

ErrorCodes InitialiseVulkan::CreateImageViews()
{
	SwapChainImageViews.Reallocate(SwapChainImages.GetSize());
	for (size_t i = 0; i < SwapChainImages.GetSize(); i++)
	{
		VkImageViewCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		createInfo.image = SwapChainImages[i];
		createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		createInfo.format = SwapChainFomat.format;
		createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		createInfo.subresourceRange.baseMipLevel = 0;
		createInfo.subresourceRange.levelCount = 1;
		createInfo.subresourceRange.baseArrayLayer = 0;
		createInfo.subresourceRange.layerCount = 1;

		if (vkCreateImageView(LogicalDevice, &createInfo, nullptr, SwapChainImageViews.GetItemAtRef(i)) != VK_SUCCESS) return ERROR;
	}

	return SUCCEEDED;
}
