#pragma once
template<typename T>
class Item
{
public:

	explicit Item(T InItem)
	{
		Value = InItem;
	}

	Item(const Item& item)
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

	void SetNext(Item* InItem)
	{
		Next = InItem;
	}

	void SetPrev(Item* InItem)
	{
		Prev = InItem;
	}

	[[nodiscard]] Item<T>* GetPrev() const
	{
		return Prev;
	}

	[[nodiscard]] Item<T>* GetNext() const
	{
		return Next;
	}

	[[nodiscard]] T GetValue() const
	{
		return Value;
	}

private:
	Item<T>* Prev = nullptr;
	Item<T>* Next = nullptr;

	T Value;
};
