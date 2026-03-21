// DO NOT MARK 
//This is because it has been submitted for my COMP305. Link to Original: https://github.falmouth.ac.uk/GA-Undergrad-Student-Work-25-26/Comp305-Engine-SL295211.git
#include "InputManager.h"
#include <GLFW/glfw3.h>

MulticastDelegate<double, double> InputManager::MouseCallback;

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
