// DO NOT MARK.
//This is because it has been submitted for my dissertation. Link to Original: https://github.falmouth.ac.uk/GA-Undergrad-Student-Work-25-26/Dissertation-SL295211.git


#include "FirstWindow.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <vulkan/vulkan_core.h>
#include <iostream>


bool FirstWindow::bFrameBufferResized = false;

FirstWindow::FirstWindow()
{
	CreateWindow();
	//glViewport(0, 0, Width, Height);
}

FirstWindow::~FirstWindow()
{
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

	glfwMakeContextCurrent(DisplayWindow);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
#if DEBUG
		std::cout << "Failed to initialise glad" << std::endl;
#endif
	}

	glfwSetWindowUserPointer(DisplayWindow, this);

	glfwSetFramebufferSizeCallback(DisplayWindow, FrameBuffer_Size_Callback);
}

void FirstWindow::OnWindowResize()
{
	glViewport(0, 0, Width, Height);
}
