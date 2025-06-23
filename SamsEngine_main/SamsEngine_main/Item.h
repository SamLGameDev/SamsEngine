#pragma once
template<typename T>
class Item
{

public:

	Item(T InItem)
	{
		Value = InItem;
	}

	Item( const Item<T>& item)
	{
		Value = item.GetValue();
	}

	~Item()
	{
		if (Next != nullptr)
		{
			Next->SetPrev(nullptr);
		}
	}

	void SetNext(Item<T>* InItem)
	{
		Next = InItem;
	}

	void SetPrev(Item<T>* InItem)
	{
		Prev = InItem;
	}

	Item<T>* GetPrev() const
	{
		return Prev;
	}

	Item<T>* GetNext() const
	{
		return Next;
	}

	T GetValue() const
	{
		return Value;
	}

private:
	Item<T>* Prev = nullptr;
	Item<T>* Next = nullptr;

	T Value;

};

