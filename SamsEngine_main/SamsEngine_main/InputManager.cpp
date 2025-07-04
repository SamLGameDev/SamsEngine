#include "InputManager.h"
#include <glad/glad.h>9
#include <GLFW/glfw3.h>


InputManager::InputManager()
{

}

InputManager::~InputManager()
{
}

void InputManager::ProcessInput(GLFWwindow* window, unsigned int Shader)
{
	if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, true);
	}
	if(glfwGetKey(window, GLFW_KEY_UP))
	{
		int UniformVisibilityLoc = glGetUniformLocation(Shader, "Visibility");
		value += 0.0001;
		if (value > 1)
		{
			value = 1;
		}
		glUniform1f(UniformVisibilityLoc, value);
	}
	if (glfwGetKey(window, GLFW_KEY_DOWN))
	{
		int UniformVisibilityLoc = glGetUniformLocation(Shader, "Visibility");

		value -= 0.0001;
		if (value < 0)
		{
			value = 0;
		}
		glUniform1f(UniformVisibilityLoc, value);
	}
}
