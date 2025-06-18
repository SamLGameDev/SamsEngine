#include "InputManager.h"
#include <GLFW/glfw3.h>

InputManager::InputManager()
{

}

InputManager::~InputManager()
{
}

void InputManager::ProcessInput(GLFWwindow* window)
{
	if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, true);
	}
}
