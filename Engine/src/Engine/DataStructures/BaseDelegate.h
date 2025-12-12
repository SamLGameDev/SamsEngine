#pragma once
#include <functional>
#include "LinkedList.h"
#include <memory>

//TODO this whole thing needs to be reworked to allow unbinding, and null checking.

template<typename... Args>
class MulticastDelegate
{

	using FuncType = std::function<void(Args...)>;
	struct FuncWrapperBase
	{
		virtual ~FuncWrapperBase() = default;
		virtual void Call(Args... args) = 0;
		virtual bool Compare(void* objPtr, const void* methodBytes) = 0;
		bool operator==(FuncWrapperBase* Other)
		{
			return Compare(Other);
		}
	};
	
	template<typename T>
	struct FuncWrapperMember : FuncWrapperBase
	{
		T* Obj;
		void(T::* Method)(Args...);

		void Call(Args... args) override
		{
			(Obj->*Method)(args...);
		}

		bool Compare(void* objPtr, const void* methodBytes) override
		{
			if (Obj != static_cast<T*>(objPtr)) return false;
			return std::memcmp(&Method, methodBytes, sizeof(Method)) == 0;
		}

		FuncWrapperMember( T* InObj, void(T::* InMethod)(Args...)) : Obj(InObj), Method(InMethod) {};
	};



public:

	MulticastDelegate() = default;

	template<typename T>
	void BindMember(T* Obj, void (T::* Method)(Args...))
	{
		std::unique_ptr<FuncWrapperBase> func = std::make_unique<FuncWrapperMember<T>>(Obj, Method);
		Funcs.Add(func);
	}

	void BindMember(const std::function<void(Args...)>& func)
	{
		//Funcs.Add(func);
	}

	void Broadcast(Args... args)
	{
		for (const auto& func : Funcs)
		{
			func->Call(args...);
		}
	}
	template<typename T>
	void Remove(T* Obj, void (T::* Method)(Args...))
	{
		const void* methodBytes = static_cast<const void*>(std::addressof(Method));
		std::unique_ptr<FuncWrapperBase> func = std::make_unique<FuncWrapperMember<T>>(Obj, Method);
		for (size_t i = 0; i<Funcs.GetSize(); i++)
		{
			if (Funcs[i]->Compare(static_cast<void*>(Obj), methodBytes))
			{
				Funcs.RemoveAt(i);
			}
		}
	}

private:

	Array<std::unique_ptr<FuncWrapperBase>> Funcs;
};
