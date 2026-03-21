// DO NOT MARK 
//This is because it has been submitted for my COMP305. Link to Original: https://github.falmouth.ac.uk/GA-Undergrad-Student-Work-25-26/Comp305-Engine-SL295211.git
#pragma once

#include "FirstWindow.h"
#include "InputManager.h"

class InputActionMouse
{
public:

	InputActionMouse() = default;

	explicit InputActionMouse(Window* Window);


	/**
	 * Binds a function to the mouse call back
	 * @tparam T The object type
	 * @param Obj Pointer to the object itself
	 * @param Method Reference to the method, using &Object::Func;
	 */
	template<typename T>
	static void BindCallback(T* Obj, void (T::* Method)(double, double));

protected:

	Window* ActiveWindow;
};

template<typename T>
inline void InputActionMouse::BindCallback(T* Obj, void(T::* Method)(double, double))
{
	InputManager::MouseCallback.BindMember<T>(Obj, Method);
}
