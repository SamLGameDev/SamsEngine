#pragma once
#include "Item.h"

template<typename T>
class LinkedList
{
public:

	LinkedList() {};

	LinkedList(const LinkedList<T>& CopyList)
	{
		Copy(CopyList);
	}

	~LinkedList()
	{
		RemoveAll();
	}

	T operator[](int Index) const
	{
		return Find(Index);
	}

	void operator=(const LinkedList<T>& CopyList)
	{
		Copy(CopyList);
	}

	T Find(int Index) const
	{
		return FindItem(Index)->GetValue();
	}

	void Copy(const LinkedList<T>& CopyList)
	{
		RemoveAll();

		FirstItem = nullptr;
		LastItem = nullptr;

		Size = 0;

		for (int i = 0; i < CopyList.GetSize(); i++)
		{
			T nextItem = CopyList.Find(i);
			Add(nextItem);
		}
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

	Item<T>* FindItem(const int Index) const
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
			for (int i = Size - 1; i > Index; i--)
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

		FirstItem = nullptr;
		LastItem = nullptr;

		while (Value != nullptr)
		{
			Item<T>* Next = Value->GetNext();
			delete Value;
			Value = Next;
		}
		Size = 0;
	}

	const bool Contains(const T InValue) const
	{
		Item<T>* Value = FirstItem;

		while (Value != nullptr)
		{
			if (Value->GetValue() == InValue)
			{
				return true;
			}
			Value = Value->GetNext();
		}
		return false;
	}

	void Sort()
	{
		bool bChanged = false;
		Item<T>* Value = FirstItem;

		while (Value != nullptr && Value->GetNext() != nullptr)
		{
			Item<T>* Next = Value->GetNext();
			if (Value->GetValue() > Next->GetValue())
			{
				// Swap Value and Next
				Item<T>* Prev = Value->GetPrev();
				Item<T>* NextNext = Next->GetNext();

				// Update links
				if (Prev) Prev->SetNext(Next);
				else FirstItem = Next;

				if (NextNext) NextNext->SetPrev(Value);
				else LastItem = Value;

				Next->SetPrev(Prev);
				Next->SetNext(Value);
				Value->SetPrev(Next);
				Value->SetNext(NextNext);

				bChanged = true;

				// Continue from the new "Next"
				// Do not advance Value here — we already moved it forward during the swap
				Value = Next;
			}
			else
			{
				Value = Value->GetNext();
			}
		}
	}

private:

	Item<T>* FirstItem = nullptr;

	Item<T>* LastItem = nullptr;

	int Size = 0;
};

