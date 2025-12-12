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

	~InputAction();

	InputAction(int InKey, InputManager* InManager, Window* InWindow);


	void ProcessInput(GLFWwindow* Window);

	MulticastDelegate<> Actions;

protected:

	InputManager* Manager;

	/**
	 * The key pressed to activate the bound actions
	 */
	int Key;
};
