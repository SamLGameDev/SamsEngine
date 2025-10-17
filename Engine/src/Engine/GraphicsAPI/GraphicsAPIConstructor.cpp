#include "GraphicsAPIConstructor.h"
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
		vulkan.Init();
	}
	return SUCCEEDED;
}
