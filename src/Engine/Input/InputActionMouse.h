#pragma once

#include "FirstWindow.h"
#include "InputManager.h"

class InputActionMouse
{
public:

	InputActionMouse() = default;

	explicit InputActionMouse(FirstWindow* Window);


	/**
	 * Binds a function to the mouse call back
	 * @tparam T The object type
	 * @param Obj Pointer to the object itself
	 * @param Method Reference to the method, using &Object::Func;
	 */
	template<typename T>
	static void BindCallback(T* Obj, void (T::* Method)(double, double));

protected:

	FirstWindow* Window;
};

template<typename T>
inline void InputActionMouse::BindCallback(T* Obj, void(T::* Method)(double, double))
{
	InputManager::MouseCallback.BindMember<T>(Obj, Method);
}
