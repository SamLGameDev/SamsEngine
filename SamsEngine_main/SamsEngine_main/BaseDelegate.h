#pragma once
#include <iostream>
#include <functional>
#include "LinkedList.h"

template<typename... Args>
class MulticastDelegate
{
	using FuncType = std::function<void(Args...)>;

public:


	MulticastDelegate() = default;

	template<typename T>
	void BindMember(T* Obj, void (T::*Method)(Args...))
	{
		FuncType InFunc = [Obj, Method](Args... args)
			{
				(Obj->*Method)(args...);
			};
		Funcs.Add(InFunc);
	}

	void Broadcast(Args... args)
	{
		for (int i = 0; i < Funcs.GetSize(); i++)
		{
			Funcs[i](args...);
		}

	}



private:

	LinkedList<FuncType> Funcs;

};

