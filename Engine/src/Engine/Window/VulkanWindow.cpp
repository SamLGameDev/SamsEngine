// DO NOT MARK 
//This is because it has been submitted for my COMP305. Link to Original: https://github.falmouth.ac.uk/GA-Undergrad-Student-Work-25-26/Comp305-Engine-SL295211.git
#include "VulkanWindow.h"
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vulkan/vulkan.h>
#include <iostream>
#include "VulkanInstance.h"

namespace Vulkan
{

	bool FirstWindow::bFrameBufferResized = false;

	FirstWindow::FirstWindow()
	{
		CreateWindow();
	}

	FirstWindow::~FirstWindow()
	{
		vkDestroySurfaceKHR(SInstance::GetInstance()->VulkanInstance, SInstance::GetInstance()->WindowsInterface, nullptr);
	}

	void FirstWindow::CreateWindow()
	{
		glfwWindowHint(GLFW_SAMPLES, AntiAliasingSamples);
		DisplayWindow = glfwCreateWindow(Width, Height, WindowName, NULL, NULL);

		if (DisplayWindow == NULL)
		{
#if DEBUG
			std::cout << "Failed to create window" << std::endl;
#endif
			glfwTerminate();
		}

		if (glfwCreateWindowSurface(SInstance::GetInstance()->VulkanInstance, DisplayWindow, nullptr, &SInstance::GetInstance()->WindowsInterface) != VK_SUCCESS)
		{
			glfwTerminate();
			return;
		}

		glfwMakeContextCurrent(DisplayWindow);

		glfwSetWindowUserPointer(DisplayWindow, this);

		glfwSetFramebufferSizeCallback(DisplayWindow, FrameBuffer_Size_Callback);

		Instance = this;
	}

	void FirstWindow::OnWindowResize()
	{
		bFrameBufferResized = true;
	}
}
