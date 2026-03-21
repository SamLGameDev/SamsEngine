// DO NOT MARK 
//This is because it has been submitted for my COMP305. Link to Original: https://github.falmouth.ac.uk/GA-Undergrad-Student-Work-25-26/Comp305-Engine-SL295211.git
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

InputAction::~InputAction()
{
	Manager->InputActions.Remove(this, &InputAction::ProcessInput);
}

InputAction::InputAction(const int InKey, InputManager* InManager, Window* InWindow)
{
	Key = InKey;
	Manager = InManager;
	Manager->InputActions.BindMember(this, &InputAction::ProcessInput);
}
