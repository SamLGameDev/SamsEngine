// DO NOT MARK.
//This is because it has been submitted for my dissertation. Link to Original: https://github.falmouth.ac.uk/GA-Undergrad-Student-Work-25-26/Dissertation-SL295211.git

#pragma once
#include "Item.h"
#include "Array.h"

template<typename T>
class LinkedList
{
public:

	LinkedList() = default;

	LinkedList(const LinkedList& CopyList)
	{
		Copy(CopyList);
	}

	~LinkedList()
	{
		RemoveAll();
	}

	[[nodiscard]] T operator[](const unsigned int Index) const
	{
		return Find(Index);
	}

	[[nodiscard]] T Find(const unsigned int Index) const
	{
		return FindItem(Index)->GetValue();
	}

	void Copy(const LinkedList& CopyList)
	{
		RemoveAll();

		FirstItem = nullptr;
		LastItem = nullptr;

		Size = 0;

		for (unsigned int i = 0; i < CopyList.GetSize(); i++)
		{
			T nextItem = CopyList.Find(i);
			Add(nextItem);
		}
	}

	LinkedList(std::initializer_list<T> Init)
	{
		for (const T item : Init)
		{
			Add(item);
		}
	}

	LinkedList& operator=(const LinkedList<T>& CopyList)
	{
		if (this != &CopyList)
		{
			Copy(CopyList);
		}
		return *this;
	}

	bool operator==(const LinkedList& other) const
	{
		if (other.GetSize() != Size)
		{
			return false;
		}

		Item<T>* value = FirstItem;
		Item<T>* otherValue = other.FirstItem;

		while (value != nullptr)
		{
			if (value->GetValue() != otherValue->GetValue())
			{
				return false;
			}
			value = value->GetNext();
			otherValue = otherValue->GetNext();
		}
		return true;

	}

	void Add(const T InItem)
	{
		auto* NewItem = new Item<T>(InItem);
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

	void Add(const Array<T>& Items)
	{
		for (int i = 0; i < Items.GetSize(); i++)
		{
			Add(Items[i]);
		}
	}

	[[nodiscard]] unsigned int GetSize() const
	{
		return Size;
	}

	[[nodiscard]] bool IsEmpty() const
	{
		if (Size == 0)
		{
			return true;
		}
		return false;
	}

	/**
	 * @return The item at that index, if it exists. Not the stored value, but the item class containig it.
	 * Use FInd instead for that
	 */
	[[nodiscard]] Item<T>* FindItem(const unsigned int Index) const
	{
		if (Index > Size - 1)
		{
			return nullptr;
		}

		Item<T>* Value;
		if (Index <= Size / 2)
		{
			Value = FirstItem;
			for (unsigned int i = 0; i < Index; i++)
			{
				Value = Value->GetNext();
			}
		}
		else
		{
			Value = LastItem;
			for (unsigned int i = Size - 1; i > Index; i--)
			{
				Value = Value->GetPrev();
			}
		}
		return Value;
	}

	void Remove(const T& Item)
	{
		unsigned int index;
		if (Contains(Item, index))
		{
			RemoveAt(index);
		}
	}

	void RemoveAt(const unsigned int Index)
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

		Size--;
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

	bool Contains(const T InValue, unsigned int& Index) const
	{
		Item<T>* Value = FirstItem;

		for (unsigned int i = 0; i < Size; i++)
		{
			if (Value->GetValue() == InValue)
			{
				Index = i;
				return true;
			}
			Value = Value->GetNext();
		}
		return false;
	}

	void Sort()
	{
		Item<T>* Value = FirstItem;

		while (Value != nullptr && Value->GetNext() != nullptr)
		{
			Item<T>* Next = Value->GetNext();
			if (Value->GetValue() > Next->GetValue())
			{
				Item<T>* Prev = Value->GetPrev();
				Item<T>* NextNext = Next->GetNext();

				if (Prev) Prev->SetNext(Next);
				else FirstItem = Next;

				if (NextNext) NextNext->SetPrev(Value);
				else LastItem = Value;

				Next->SetPrev(Prev);
				Next->SetNext(Value);
				Value->SetPrev(Next);
				Value->SetNext(NextNext);

				Value = Next;
			}
			else
			{
				Value = Value->GetNext();
			}
		}
	}

	class Iterator
	{
	private:
		Item<T>* Current;

	public:
		explicit Iterator(Item<T>* node) : Current(node) {}

		[[nodiscard]] T operator*() { return Current->GetValue(); }
		[[nodiscard]] T operator->() { return &Current->GetValue(); }

		Iterator& operator++() {
			Current = Current->GetNext();
			return *this;
		}

		[[nodiscard]] bool operator!=(const Iterator& other) const {
			return Current != other.Current;
		}

	};

	[[nodiscard]] Iterator begin() { return Iterator(FirstItem); }
	[[nodiscard]] Iterator end() { return Iterator(LastItem); }

private:

	Item<T>* FirstItem = nullptr;

	Item<T>* LastItem = nullptr;

	unsigned int Size = 0;
};
