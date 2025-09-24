#include "InputManager.h"
#include <GLFW/glfw3.h>

MulticastDelegate<float, float> InputManager::MouseCallback;

InputManager::InputManager(GLFWwindow* Window)
{
	glfwSetCursorPosCallback(Window, mouse_callback);
}


void InputManager::ProcessInput(GLFWwindow* window)
{
	InputActions.Broadcast(window);
}

void mouse_callback(GLFWwindow* Window, const double Xpos, const double Ypos)
{
	InputManager::MouseCallback.Broadcast(Xpos, Ypos);
}