#include "FirstWindow.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>

float FirstWindow::Width;
float FirstWindow::Height;

FirstWindow::FirstWindow()
{
	Initialisation();
	CreateWindow();
	glViewport(0, 0, 800, 600);
	Width = 800;
	Height = 600;
}

FirstWindow::~FirstWindow()
{
}

void FirstWindow::Initialisation()
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
}

void FirstWindow::CreateWindow()
{
	Window = glfwCreateWindow(800, 600, "FirstWindow", NULL, NULL);
	if (Window == NULL)
	{
		std::cout << "Failed to create window" << std::endl;
		glfwTerminate();
	}

	glfwMakeContextCurrent(Window);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialise glad" << std::endl;
	}

	glfwSetFramebufferSizeCallback(Window, FrameBuffer_Size_Callback);
}

void FirstWindow::FrameBuffer_Size_Callback(GLFWwindow* Inwindow, int InWidth, int InHeight)
{
	glViewport(0, 0, InWidth, InHeight);
	FirstWindow::Width = InWidth;
	FirstWindow::Height = InHeight;
}
