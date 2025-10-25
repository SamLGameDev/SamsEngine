#include "FirstWindow.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <vulkan/vulkan_core.h>
#include <iostream>

GLint FirstWindow::Width = 800, FirstWindow::Height = 600;

GLFWwindow* FirstWindow::Window;

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

	glfwSetFramebufferSizeCallback(Window, FrameBuffer_Size_Callback);
}

void FirstWindow::FrameBuffer_Size_Callback(GLFWwindow* Inwindow, const int InWidth, const int InHeight)
{
	//TODO make this have a static delegate call, that will call all functions that need adjusting with frame size, like FBOS

	glViewport(0, 0, InWidth, InHeight);
	FirstWindow::Width = InWidth;
	FirstWindow::Height = InHeight;
}
