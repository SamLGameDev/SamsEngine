#include "FirstWindow.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <vulkan/vulkan_core.h>
#include "InitialiseVulkan.h"
#include <iostream>

GLint FirstWindow::Width = 800, FirstWindow::Height = 600;

FirstWindow::FirstWindow()
{
	Initialisation(Vulkan);
	CreateWindow();
	//glViewport(0, 0, Width, Height);
}

FirstWindow::~FirstWindow()
{
}

void FirstWindow::Initialisation(const GraphicsAPI& API)
{
	glfwInit();

	if (API == OpenGl)
	{
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, static_cast<int>(OpenGLVersion));

		//do this to get the first digit
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, static_cast<int>(OpenGLVersion * 10) % 10);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	}


	if (API == Vulkan)
	{
		InitialiseVulkan vulkan;
		vulkan.Init();
	}
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


	while (!glfwWindowShouldClose(Window)) {
		glfwPollEvents();
	}
}

void FirstWindow::FrameBuffer_Size_Callback(GLFWwindow* Inwindow, const int InWidth, const int InHeight)
{
	//TODO make this have a static delegate call, that will call all functions that need adjusting with frame size, like FBOS

	glViewport(0, 0, InWidth, InHeight);
	FirstWindow::Width = InWidth;
	FirstWindow::Height = InHeight;
}
