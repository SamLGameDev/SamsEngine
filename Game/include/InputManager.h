// DO NOT MARK 
//This is because it has been submitted for my COMP305. Link to Original: https://github.falmouth.ac.uk/GA-Undergrad-Student-Work-25-26/Comp305-Engine-SL295211.git
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

	static MulticastDelegate<double, double> MouseCallback;
};

void mouse_callback(GLFWwindow* Window, double Xpos, double Ypos);
