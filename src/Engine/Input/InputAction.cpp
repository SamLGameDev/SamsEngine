#include "InputAction.h"
#include "InputManager.h"

void InputAction::ProcessInput(GLFWwindow* Window)
{
	if (glfwGetKey(Window, Key) == GLFW_PRESS)
	{
		Actions.Broadcast();
	}
}

InputAction::InputAction(const int InKey, InputManager* Manager, FirstWindow* InWindow)
{
	Key = InKey;
	Manager->InputActions.BindMember(this, &InputAction::ProcessInput);
}