#include "GraphicsAPIConstructor.h"
#include<glad/glad.h>
#include "GLFW/glfw3.h"


ErrorCodes GraphicsAPIConstructor::Init(const GraphicsAPI& Api)
{
	if (Api == OpenGl)
	{
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, static_cast<int>(OpenGLVersion));

		//do this to get the first digit
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, static_cast<int>(OpenGLVersion * 10) % 10);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	}


	if (Api == Vulkan)
	{
		vulkan = new InitialiseVulkan();
		vulkan->Init();
	}
	return SUCCEEDED;
}

void GraphicsAPIConstructor::AttachToWindow(const GraphicsAPI& Api)
{
	if (Api == Vulkan)
	{
		vulkan->AttachToWindow();
	}
}

void GraphicsAPIConstructor::Shutdown(const GraphicsAPI& API)
{
	if (API == Vulkan)
	{
		delete vulkan;
	}
}
