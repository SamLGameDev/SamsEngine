#pragma once
template<typename T>
class Item
{

public:

	Item(T InItem)
	{
		Value = InItem;
	}

	~Item()
	{

	}

	void SetNext(Item<T>* InItem)
	{
		Next = InItem;
	}

	void SetPrev(Item<T>* InItem)
	{
		Prev = InItem;
	}

	Item<T>* GetPrev()
	{
		return Prev;
	}

	Item<T>* GetNext()
	{
		return Next;
	}

	T GetValue()
	{
		return Value;
	}

private:
	Item<T>* Prev = nullptr;
	Item<T>* Next = nullptr;

	T Value;

};

