#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "BaseDelegate.h"

class Window;
class FirstWindow;
class InputManager;

class InputAction
{
public:

	InputAction() = default;

	InputAction(int InKey, InputManager* Manager, Window* InWindow);


	void ProcessInput(GLFWwindow* Window);

	MulticastDelegate<> Actions;

protected:


	/**
	 * The key pressed to activate the bound actions
	 */
	int Key;
};
