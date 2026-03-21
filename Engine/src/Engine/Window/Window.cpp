// DO NOT MARK 
//This is because it has been submitted for my COMP305. Link to Original: https://github.falmouth.ac.uk/GA-Undergrad-Student-Work-25-26/Comp305-Engine-SL295211.git
#include "Window.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <vulkan/vulkan_core.h>
#include <iostream>

GLint Window::Width = 800, Window::Height = 600;

Window* Window::Instance = nullptr;

GLFWwindow* Window::DisplayWindow;

bool Window::bFrameBufferResized = false;

void Window::FrameBuffer_Size_Callback(GLFWwindow* Inwindow, const int InWidth, const int InHeight)
{
	//TODO make this have a static delegate call, that will call all functions that need adjusting with frame size, like FBOS

	//glViewport(0, 0, InWidth, InHeight);

	Window::Width = InWidth;
	Window::Height = InHeight;

	Window* window = reinterpret_cast<Window*>(glfwGetWindowUserPointer(Inwindow));

	window->OnWindowResize();
}
