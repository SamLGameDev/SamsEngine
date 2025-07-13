#pragma once

#include <GLFW/glfw3.h>
#include "BaseDelegate.h"
#include <any>

class FirstWindow;
class InputManager;

class InputAction
{
public:

	InputAction();

	InputAction(const int InKey, InputManager* Manager, FirstWindow* InWindow);
	/// <summary>
	/// 
	/// </summary>
	void ProcessInput(GLFWwindow* Window);

	MulticastDelegate<> Actions;

protected:

	int Key;

};

