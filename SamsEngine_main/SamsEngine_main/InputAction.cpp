#include "InputAction.h"
#include "InputManager.h"
#include "FirstWindow.h"
#include "InputActionMouse.h"

InputAction::InputAction()
{
}



void InputAction::ProcessInput(GLFWwindow* Window)
{
	if (glfwGetKey(Window,Key) == GLFW_PRESS)
	{
		Actions.Broadcast();
	}
}

InputAction::InputAction(const int InKey, InputManager* Manager, FirstWindow* InWindow)
{
	Key = InKey;
	Manager->InputActions.BindMember(this, &InputAction::ProcessInput);

}
