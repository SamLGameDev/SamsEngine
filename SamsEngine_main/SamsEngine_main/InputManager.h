#pragma once

struct GLFWwindow;

class InputManager
{
public:
	InputManager();
	~InputManager();

	void ProcessInput(GLFWwindow* window, unsigned int Shader);

private:
	float value = 0;
};

