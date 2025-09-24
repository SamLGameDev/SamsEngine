#pragma once

#include "BaseDelegate.h"

struct GLFWwindow;

class InputManager
{
public:

	explicit InputManager(GLFWwindow* Window);
	~InputManager() = default;

	void ProcessInput(GLFWwindow* window);

	MulticastDelegate<GLFWwindow*> InputActions;

	static MulticastDelegate<float, float> MouseCallback;
};

void mouse_callback(GLFWwindow* Window, double Xpos, double Ypos);
