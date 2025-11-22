#include "InputAction.h"
#include "InputManager.h"
#include "Window.h"

void InputAction::ProcessInput(GLFWwindow* Window)
{
	if (glfwGetKey(Window, Key) == GLFW_PRESS)
	{
		Actions.Broadcast();
	}
}

InputAction::InputAction(const int InKey, InputManager* Manager, Window* InWindow)
{
	Key = InKey;
	Manager->InputActions.BindMember(this, &InputAction::ProcessInput);
}
