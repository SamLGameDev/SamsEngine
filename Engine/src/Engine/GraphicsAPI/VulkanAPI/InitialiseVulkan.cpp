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


	std::ifstream File(CorePaths::Contents.Path + "\\Shaders\\triangleVert.spv",  std::ios::binary);
	File.open(CorePaths::Contents.Path + "\\Shaders\\triangleVert.spv");
	std::stringstream Buffer;
	Buffer << File.rdbuf();
	File.close();



	std::ifstream Frag(CorePaths::Contents.Path + "\\Shaders\\triangleFrag.spv", std::ios::binary);
	Frag.open(CorePaths::Contents.Path + "\\Shaders\\triangleFrag.spv");
	std::stringstream FBuffer;
	FBuffer << Frag.rdbuf();
	Frag.close();

	std::string VertCode = Buffer.str();

	VkShaderModuleCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	createInfo.codeSize = VertCode.size();
	createInfo.pCode = reinterpret_cast<const uint32_t*>(VertCode.data());

	VkShaderModule vertShader;

	vkCreateShaderModule(LogicalDevice, &createInfo, nullptr, &vertShader);

	std::string FragCode = FBuffer.str();

	//std::cout << FragCode << "\n";

	//VkShaderModuleCreateInfo createInfo{};
	//createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	createInfo.codeSize = FragCode.size();
	createInfo.pCode = reinterpret_cast<const uint32_t*>(FragCode.data());

	VkShaderModule fragShader;

	vkCreateShaderModule(LogicalDevice, &createInfo, nullptr, &fragShader);

	VkPipelineShaderStageCreateInfo vertPipelineCreateInfo{};
	vertPipelineCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	vertPipelineCreateInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
	vertPipelineCreateInfo.module = vertShader;
	vertPipelineCreateInfo.pName = "main";

	VkPipelineShaderStageCreateInfo fragPipelineCreateInfo{};
	fragPipelineCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	fragPipelineCreateInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
	fragPipelineCreateInfo.module = fragShader;
	fragPipelineCreateInfo.pName = "main";

	Array<VkPipelineShaderStageCreateInfo> Shaders = {vertPipelineCreateInfo, fragPipelineCreateInfo};

	Array<VkDynamicState> dynamicStates = {VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR};

	VkPipelineDynamicStateCreateInfo dynamicStatesCreateInfo{};
	dynamicStatesCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
	dynamicStatesCreateInfo.dynamicStateCount = dynamicStates.GetSize();
	dynamicStatesCreateInfo.pDynamicStates = dynamicStates.GetFirstRef();

	VkPipelineVertexInputStateCreateInfo vertexInputStateCreateInfo{};
	vertexInputStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
	vertexInputStateCreateInfo.pVertexAttributeDescriptions = nullptr;
	vertexInputStateCreateInfo.pVertexBindingDescriptions = nullptr;
	vertexInputStateCreateInfo.vertexAttributeDescriptionCount = 0;
	vertexInputStateCreateInfo.vertexBindingDescriptionCount = 0;

	VkPipelineInputAssemblyStateCreateInfo inputAssemblyCreateInfo{};
	inputAssemblyCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	inputAssemblyCreateInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
	inputAssemblyCreateInfo.primitiveRestartEnable = VK_FALSE;

	VkViewport viewport{};
	viewport.x = 0;
	viewport.y = 0;
	viewport.width = SwapChainExtent.width;
	viewport.height = SwapChainExtent.height;
	viewport.minDepth = 0;
	viewport.maxDepth = 1;

	VkRect2D scissor;
	scissor.extent = SwapChainExtent;
	scissor.offset = {0, 0};

	VkPipelineViewportStateCreateInfo viewportStateCreateInfo{};
	viewportStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	//viewportStateCreateInfo.pViewports = &viewport;
	//viewportStateCreateInfo.pScissors = &scissor;
	viewportStateCreateInfo.viewportCount = 1;
	viewportStateCreateInfo.scissorCount = 1;

	VkPipelineRasterizationStateCreateInfo rasterizationCreateInfo{};
	rasterizationCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	rasterizationCreateInfo.depthClampEnable = VK_FALSE;
	rasterizationCreateInfo.rasterizerDiscardEnable = VK_FALSE;
	rasterizationCreateInfo.polygonMode = VK_POLYGON_MODE_FILL;
	rasterizationCreateInfo.lineWidth = 1;
	rasterizationCreateInfo.cullMode = VK_CULL_MODE_BACK_BIT;
	rasterizationCreateInfo.frontFace = VK_FRONT_FACE_CLOCKWISE;
	rasterizationCreateInfo.depthBiasEnable = VK_FALSE;
	rasterizationCreateInfo.depthBiasConstantFactor = 0;
	rasterizationCreateInfo.depthBiasSlopeFactor = 0;
	rasterizationCreateInfo.depthBiasClamp = 0;

	VkPipelineMultisampleStateCreateInfo multisampleCreateInfo{};
	multisampleCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	multisampleCreateInfo.sampleShadingEnable = VK_FALSE;
	multisampleCreateInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
	multisampleCreateInfo.minSampleShading = 1;
	multisampleCreateInfo.pSampleMask = nullptr;
	multisampleCreateInfo.alphaToCoverageEnable = VK_FALSE;
	multisampleCreateInfo.alphaToOneEnable = VK_FALSE;

	VkPipelineColorBlendAttachmentState colorBlendAttachmentState{};
	colorBlendAttachmentState.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
	colorBlendAttachmentState.blendEnable = VK_FALSE;
	colorBlendAttachmentState.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;
	colorBlendAttachmentState.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;
	colorBlendAttachmentState.colorBlendOp = VK_BLEND_OP_ADD;
	colorBlendAttachmentState.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
	colorBlendAttachmentState.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
	colorBlendAttachmentState.alphaBlendOp = VK_BLEND_OP_ADD;

	VkPipelineColorBlendStateCreateInfo colorBlendStateCreateInfo{};
	colorBlendStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	colorBlendStateCreateInfo.logicOpEnable = VK_FALSE;
	colorBlendStateCreateInfo.logicOp = VK_LOGIC_OP_COPY;
	colorBlendStateCreateInfo.attachmentCount = 1;
	colorBlendStateCreateInfo.pAttachments = &colorBlendAttachmentState;
	colorBlendStateCreateInfo.blendConstants[0] = 0;
	colorBlendStateCreateInfo.blendConstants[1] = 0;
	colorBlendStateCreateInfo.blendConstants[2] = 0;
	colorBlendStateCreateInfo.blendConstants[3] = 0;

	VkPipelineLayoutCreateInfo layoutCreateInfo{};
	layoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	layoutCreateInfo.setLayoutCount = 0;
	layoutCreateInfo.pushConstantRangeCount = 0;
	layoutCreateInfo.pPushConstantRanges = nullptr;
	layoutCreateInfo.pSetLayouts = nullptr;

	vkCreatePipelineLayout(LogicalDevice, &layoutCreateInfo, nullptr, &Layout);

	VkAttachmentDescription colorAttachment{};
	colorAttachment.format = SwapChainFomat.format;
	colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
	colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

	VkAttachmentReference colorAttachmentRef{};
	colorAttachmentRef.attachment = 0;
	colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	VkSubpassDescription subpassDescription{};
	subpassDescription.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	subpassDescription.colorAttachmentCount = 1;
	subpassDescription.pColorAttachments = &colorAttachmentRef;

	VkRenderPassCreateInfo renderPassCreateInfo{};
	renderPassCreateInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	renderPassCreateInfo.attachmentCount = 1;
	renderPassCreateInfo.pAttachments = &colorAttachment;
	renderPassCreateInfo.subpassCount = 1;
	renderPassCreateInfo.pSubpasses = &subpassDescription;

	VkSubpassDependency subpassDependency{};
	subpassDependency.srcSubpass = VK_SUBPASS_EXTERNAL;
	subpassDependency.dstSubpass = 0;
	subpassDependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	subpassDependency.srcAccessMask = 0;
	subpassDependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	subpassDependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

	renderPassCreateInfo.dependencyCount = 1;
	renderPassCreateInfo.pDependencies= &subpassDependency;

	vkCreateRenderPass(LogicalDevice, &renderPassCreateInfo, nullptr, &RenderPass);

	VkGraphicsPipelineCreateInfo pipelineCreateInfo{};
	pipelineCreateInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	pipelineCreateInfo.stageCount = 2;
	pipelineCreateInfo.pStages = Shaders.GetFirstRef();
	pipelineCreateInfo.pInputAssemblyState = &inputAssemblyCreateInfo;
	pipelineCreateInfo.pVertexInputState = &vertexInputStateCreateInfo;
	pipelineCreateInfo.pViewportState = &viewportStateCreateInfo;
	pipelineCreateInfo.pRasterizationState = &rasterizationCreateInfo;
	pipelineCreateInfo.pColorBlendState = &colorBlendStateCreateInfo;
	pipelineCreateInfo.pDepthStencilState = nullptr;
	pipelineCreateInfo.pDynamicState = &dynamicStatesCreateInfo;
	pipelineCreateInfo.pMultisampleState = &multisampleCreateInfo;
	pipelineCreateInfo.layout = Layout;
	pipelineCreateInfo.renderPass = RenderPass;
	pipelineCreateInfo.subpass = 0;
	pipelineCreateInfo.basePipelineHandle = VK_NULL_HANDLE;
	pipelineCreateInfo.basePipelineIndex = -1;

	vkCreateGraphicsPipelines(LogicalDevice, VK_NULL_HANDLE, 1, &pipelineCreateInfo, nullptr, &GraphicsPipeline);

	vkDestroyShaderModule(LogicalDevice, vertShader, nullptr);
	vkDestroyShaderModule(LogicalDevice, fragShader, nullptr);

	CreateFrameBuffer();

	CreateCommandPool();
	CreateCommandBuffers();
	if (CreateSyncObjects() == ERROR) std::cout << "HELp \n";

	size_t currentFrame = 0;

	while (!glfwWindowShouldClose(FirstWindow::Window))
	{
		glfwPollEvents();

		vkWaitForFences(LogicalDevice, 1, InFlightFences.GetItemAtRef(currentFrame), VK_TRUE, UINT64_MAX);

		uint32_t imageIndex;
		VkResult result = vkAcquireNextImageKHR(LogicalDevice, SwapChain, UINT64_MAX, ImageAvailableSemaphores[currentFrame], VK_NULL_HANDLE, &imageIndex);

		if (result == VK_ERROR_OUT_OF_DATE_KHR)
		{
			RecreateSwapChain();
			continue;
		}
		else if ( result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
		{
			return;
		}

		vkResetFences(LogicalDevice, 1, InFlightFences.GetItemAtRef(currentFrame));

		vkResetCommandBuffer(CommandBuffers[currentFrame], 0);

		RecordCommandBuffer(CommandBuffers[currentFrame], imageIndex);

		VkSubmitInfo submitInfo{};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

		VkSemaphore waitSemaphores[] = { ImageAvailableSemaphores[currentFrame] };

		VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
		submitInfo.waitSemaphoreCount = 1;
		submitInfo.pWaitSemaphores = waitSemaphores;
		submitInfo.pWaitDstStageMask = waitStages;

		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = CommandBuffers.GetItemAtRef(currentFrame);

		VkSemaphore signalSemaphores[] = {RenderFinishedSemephores[currentFrame]};

		submitInfo.signalSemaphoreCount = 1;
		submitInfo.pSignalSemaphores = signalSemaphores;

		if (vkQueueSubmit(GraphicsQueue, 1, &submitInfo, InFlightFences[currentFrame]) != VK_SUCCESS) break;

		VkPresentInfoKHR presentInfo{};
		presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
		presentInfo.pImageIndices = &imageIndex;

		VkSwapchainKHR swapChains[] = {SwapChain};

		presentInfo.pSwapchains = swapChains;
		presentInfo.swapchainCount = 1;
		presentInfo.waitSemaphoreCount = 1;
		presentInfo.pWaitSemaphores = signalSemaphores;
		presentInfo.pResults = nullptr;

		result = vkQueuePresentKHR(GraphicsQueue, &presentInfo);

		if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || FirstWindow::HasWindowBeenResized())
		{
			FirstWindow::ResetWindowResize();
			RecreateSwapChain();
		}
		else if (result != VK_SUCCESS) return;

		currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;

	}

	vkDeviceWaitIdle(LogicalDevice);

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

void InitialiseVulkan::CleanUpSwapChain()
{
	for (auto framebuffer : SwapChainFrameBuffers)
	{
		vkDestroyFramebuffer(LogicalDevice, framebuffer, nullptr);
	}


	for (VkImageView& view : SwapChainImageViews)
	{
		vkDestroyImageView(LogicalDevice, view, nullptr);
	}

	vkDestroySwapchainKHR(LogicalDevice, SwapChain, nullptr);
}

ErrorCodes InitialiseVulkan::Shutdown()
{

	for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
	{
		vkDestroySemaphore(LogicalDevice, ImageAvailableSemaphores.GetItemAt(i), nullptr);

		vkDestroySemaphore(LogicalDevice, RenderFinishedSemephores.GetItemAt(i), nullptr);

		vkDestroyFence(LogicalDevice, InFlightFences.GetItemAt(i), nullptr);
	}



	vkDestroyCommandPool(LogicalDevice, CommandPool, nullptr);

	vkDestroyPipeline(LogicalDevice, GraphicsPipeline, nullptr);
	vkDestroyPipelineLayout(LogicalDevice, Layout, nullptr);
	vkDestroyRenderPass(LogicalDevice, RenderPass, nullptr);


	CleanUpSwapChain();

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

ErrorCodes InitialiseVulkan::CreateCommandPool()
{
	VkCommandPoolCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	createInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
	createInfo.queueFamilyIndex = Indices.GraphicsFamily.value();

	if (vkCreateCommandPool(LogicalDevice, &createInfo, nullptr, &CommandPool) != VK_SUCCESS)
	{
		return ERROR;
	}
	return SUCCEEDED;
}

ErrorCodes InitialiseVulkan::CreateCommandBuffers()
{
	CommandBuffers.Reallocate(MAX_FRAMES_IN_FLIGHT);

	VkCommandBufferAllocateInfo allocateInfo{};
	allocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocateInfo.commandBufferCount = CommandBuffers.GetSize();
	allocateInfo.commandPool = CommandPool;
	allocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	if (vkAllocateCommandBuffers(LogicalDevice, &allocateInfo, CommandBuffers.GetFirstRef()) != VK_SUCCESS) return ERROR;
	
	return SUCCEEDED;
}

ErrorCodes InitialiseVulkan::RecordCommandBuffer(VkCommandBuffer Buffer, std::uint32_t ImageIndex)
{
	VkCommandBufferBeginInfo beginInfo{};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.pInheritanceInfo = nullptr;
	beginInfo.flags = 0;

	if (vkBeginCommandBuffer(Buffer, &beginInfo) != VK_SUCCESS) return ERROR;

	VkRenderPassBeginInfo renderBeginInfo{};
	renderBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	renderBeginInfo.renderPass = RenderPass;
	renderBeginInfo.framebuffer = SwapChainFrameBuffers[ImageIndex];
	renderBeginInfo.renderArea.offset = {0, 0};
	renderBeginInfo.renderArea.extent = SwapChainExtent;
	VkClearValue clearValue = { {{0, 0, 0, 1.f}} };
	renderBeginInfo.clearValueCount = 1;
	renderBeginInfo.pClearValues = &clearValue;

	vkCmdBeginRenderPass(Buffer, &renderBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

	vkCmdBindPipeline(Buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, GraphicsPipeline);


	VkViewport viewport{};
	viewport.x = 0;
	viewport.y = 0;
	viewport.width = SwapChainExtent.width;
	viewport.height = SwapChainExtent.height;
	viewport.minDepth = 0;
	viewport.maxDepth = 1;

	vkCmdSetViewport(Buffer, 0, 1, &viewport);

	VkRect2D scissor;
	scissor.extent = SwapChainExtent;
	scissor.offset = { 0, 0 };
	vkCmdSetScissor(Buffer, 0, 1, &scissor);

	vkCmdDraw(Buffer, 3, 1, 0, 0);

	vkCmdEndRenderPass(Buffer);

	if (vkEndCommandBuffer(Buffer) != VK_SUCCESS) return ERROR;


	return SUCCEEDED;


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

ErrorCodes InitialiseVulkan::CreateFrameBuffer()
{
	SwapChainFrameBuffers.Reallocate(SwapChainImageViews.GetSize());

	for (size_t i = 0; i < SwapChainImageViews.GetSize(); i++)
	{
		VkImageView attachments[] = { SwapChainImageViews[i] };


		VkFramebufferCreateInfo framebufferCreateInfo{};
		framebufferCreateInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		framebufferCreateInfo.renderPass = RenderPass;
		framebufferCreateInfo.attachmentCount = 1;
		framebufferCreateInfo.pAttachments = attachments;
		framebufferCreateInfo.width = SwapChainExtent.width;
		framebufferCreateInfo.height = SwapChainExtent.height;
		framebufferCreateInfo.layers = 1;

		if (vkCreateFramebuffer(LogicalDevice, &framebufferCreateInfo, nullptr, SwapChainFrameBuffers.GetItemAtRef(i)) != VK_SUCCESS) return ERROR;
	}
	return SUCCEEDED;
}

ErrorCodes InitialiseVulkan::CreateSyncObjects()
{

	ImageAvailableSemaphores.Reallocate(MAX_FRAMES_IN_FLIGHT);
	RenderFinishedSemephores.Reallocate(MAX_FRAMES_IN_FLIGHT);
	InFlightFences.Reallocate(MAX_FRAMES_IN_FLIGHT);

	VkSemaphoreCreateInfo semaphoreCreateInfo{};
	semaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

	VkFenceCreateInfo fenceCreateInfo{};
	fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	fenceCreateInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

	for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
	{
		if (vkCreateSemaphore(LogicalDevice, &semaphoreCreateInfo, nullptr, ImageAvailableSemaphores.GetItemAtRef(i)) != VK_SUCCESS) return ERROR;

		if (vkCreateSemaphore(LogicalDevice, &semaphoreCreateInfo, nullptr, RenderFinishedSemephores.GetItemAtRef(i)) != VK_SUCCESS) return ERROR;
		if (vkCreateFence(LogicalDevice, &fenceCreateInfo, nullptr, InFlightFences.GetItemAtRef(i)) != VK_SUCCESS) return ERROR;
	}

	return SUCCEEDED;
}

ErrorCodes InitialiseVulkan::RecreateSwapChain()
{
	int width, height = 0;

	glfwGetFramebufferSize(FirstWindow::Window, &width, &height);

	while (width == 0 || height == 0)
	{

		glfwGetFramebufferSize(FirstWindow::Window, &width, &height);
		glfwWaitEvents();
	}

	vkDeviceWaitIdle(LogicalDevice);

	CleanUpSwapChain();

	CreateSwapChain();
	CreateImageViews();
	CreateFrameBuffer();

	return SUCCEEDED;
}
