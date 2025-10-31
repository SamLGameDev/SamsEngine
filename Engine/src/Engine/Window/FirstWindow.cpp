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
	Window = glfwCreateWindow(Width, Height, WindowName, NULL, NULL);

	if (Window == NULL)
	{
#if DEBUG
		std::cout << "Failed to create window" << std::endl;
#endif
		glfwTerminate();
	}

	glfwMakeContextCurrent(Window);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
#if DEBUG
		std::cout << "Failed to initialise glad" << std::endl;
#endif
	}

	glfwSetWindowUserPointer(Window, this);

	glfwSetFramebufferSizeCallback(Window, FrameBuffer_Size_Callback);
}

void FirstWindow::OnWindowResize()
{
	glViewport(0, 0, Width, Height);
}
