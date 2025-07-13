#pragma once

#include "BaseDelegate.h"

struct GLFWwindow;

class InputManager
{
public:
	InputManager(GLFWwindow* Window);
	~InputManager();

	void ProcessInput(GLFWwindow* window, unsigned int Shader);

	MulticastDelegate<GLFWwindow*> InputActions;

	static MulticastDelegate<float, float> MouseCallback;

private:

	float value = 0;
};

void mouse_callback(GLFWwindow* window, double xpos, double ypos);

