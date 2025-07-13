#pragma once

#include "FirstWindow.h"
#include "InputManager.h"

class InputActionMouse
{
public:

	InputActionMouse();

	InputActionMouse(FirstWindow* Window);

	template<typename T>
	void BindCallback(T* Obj, void (T::*Method)(float, float));

protected:

	FirstWindow* Window;
};

template<typename T>
inline void InputActionMouse::BindCallback(T* Obj, void(T::* Method)(float, float))
{
	InputManager::MouseCallback.BindMember(Obj, Method);
}
