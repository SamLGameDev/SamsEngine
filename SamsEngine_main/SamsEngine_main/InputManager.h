#pragma once

struct GLFWwindow;

class InputManager
{
public:
	InputManager();
	~InputManager();

	void ProcessInput(GLFWwindow* window);
};

