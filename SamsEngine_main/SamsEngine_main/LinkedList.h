#pragma once
#include "Item.h"

template<typename T>
class LinkedList
{
public:

	LinkedList() {};

	~LinkedList()
	{
		RemoveAll();
	}

	T operator[](int Index)
	{
		return Find(Index);
	}

	T Find(int Index)
	{
		return FindItem(Index)->GetValue();
	}

	void Add(T InItem)
	{
		Item<T>* NewItem = new Item<T>(InItem);
		if (LastItem == nullptr)
		{
			FirstItem = NewItem;
			LastItem = NewItem;
		}
		else
		{
			LastItem->SetNext(NewItem);
			NewItem->SetPrev(LastItem);
			LastItem = NewItem;
		}
		Size++;

	}

	const int GetSize() const
	{
		return Size;
	}

	const bool IsEmpty() const
	{
		if (Size == 0)
		{
			return true;
		}
		return false;
	}

	Item<T>* FindItem(const int Index)
	{
		if (Index > Size - 1)
		{
			return nullptr;
		}

		Item<T>* Value;
		if (Index <= Size / 2)
		{
			Value = FirstItem;
			for (int i = 0; i < Index; i++)
			{
				Value = Value->GetNext();
			}
		}
		else
		{
			Value = LastItem;
			for (int i = Size; i > Index; i--)
			{
				Value = Value->GetPrev();
			}
		}
		return Value;
	}

	void RemoveAt(const int Index)
	{
		Item<T>* item = FindItem(Index);

		if (item == nullptr)
		{
			return;
		}

		if (item->GetPrev() != nullptr)
		{
			item->GetPrev()->SetNext(item->GetNext());
		}
		else
		{
			FirstItem = item->GetNext();
		}

		if (item->GetNext() != nullptr)
		{
			item->GetNext()->SetPrev(item->GetPrev());
		}
		else
		{
			LastItem = item->GetPrev();
		}

		delete item;

	}

	void RemoveAll()
	{
		Item<T>* Value = FirstItem;
		for (int i = 0; i < Size; i++)
		{
			Item<T>* Removed = Value;
			Value = Value->GetNext();
			delete Removed;
		}
	}

private:

	Item<T>* FirstItem = nullptr;

	Item<T>* LastItem = nullptr;

	int Size = 0;
};

