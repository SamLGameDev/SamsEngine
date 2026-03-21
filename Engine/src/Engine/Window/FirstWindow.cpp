


#include "FirstWindow.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <vulkan/vulkan_core.h>
#include <iostream>
namespace OpenGL {

	bool FirstWindow::bFrameBufferResized = false;

	FirstWindow::FirstWindow()
	{
		CreateWindow();
	}

	FirstWindow::~FirstWindow()
	{
	}

	void FirstWindow::CreateWindow()
	{
		glfwWindowHint(GLFW_SAMPLES, AntiAliasingSamples);
		glfwWindowHint(GLFW_DEPTH_BITS, 64);
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
			std::cout << "Failed to initialise glad" << std::endl;;
#endif
		}

		glfwSetWindowUserPointer(DisplayWindow, this);

		glfwSetFramebufferSizeCallback(DisplayWindow, FrameBuffer_Size_Callback);

		glViewport(0, 0, Width, Height);

		Instance = this;
	}

	void FirstWindow::OnWindowResize()
	{
		glViewport(0, 0, Width, Height);
		bFrameBufferResized = true;
	}
}
