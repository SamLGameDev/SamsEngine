#pragma once
#include <iostream>
template<typename T>
class Array
{
public:
	Array()
	{
		ArraySize = 0;
		DynamicArray = new T[1];
	}
	~Array()
	{
		delete[] DynamicArray;
	}

	Array(const Array<T>& CopyArray)
	{
		copy(CopyArray);
	}

	Array& operator=(const Array& other)
	{
		if (this != &other)
		{ 
			copy(other);
		}
		return *this;
	}

	T operator[](const int Index) const
	{
		return GetItemAt(Index);
	}

	T* GetItemAtRef(const int Index) const
	{
		return &DynamicArray[Index];
	}

	T* GetFirstRef()const
	{
		return DynamicArray;
	}

	void Add(T item)
	{
		T* NewArray = new T[ArraySize + 1];

		for (int i = 0; i < ArraySize; i++)
		{
			NewArray[i] = GetItemAt(i);
		}
		NewArray[ArraySize] = item;
		ArraySize++;

		delete[] DynamicArray;
		DynamicArray = NewArray;

	}


	void Add(Array<T>& item)
	{
		T* NewArray = new T[ArraySize + item.GetSize()];

		for (unsigned int i = 0; i < ArraySize; i++)
		{
			NewArray[i] = GetItemAt(i);
		}

		for (unsigned int i = 0; i < item.GetSize(); i++)
		{
			NewArray[i + ArraySize] = item.GetItemAt(i);
		}

		ArraySize += item.GetSize();;

		delete[] DynamicArray;
		DynamicArray = NewArray;

	}

	T GetItemAt(int Index) const
	{
		return DynamicArray[Index];
	}

	const unsigned int GetSize() const
	{
		return ArraySize;
	}

	T GetArray()
	{
		return DynamicArray;
	}

	const bool Contains(T& Item, unsigned int& Index)
	{
		for (unsigned int i = 0; i < ArraySize; i++)
		{
			if (Item == DynamicArray[i])
			{
				Index = i;
				return true;
			}
		}
		return false;
	}

private:

	void copy(const Array& other)
	{
		delete[] DynamicArray;
		DynamicArray = new T[other.GetSize()];
		for (int i = 0; i < other.GetSize(); i++)
		{
			DynamicArray[i] = other.GetItemAt(i);
		}
		ArraySize = other.GetSize();
	}

	T* DynamicArray = nullptr;

	unsigned int ArraySize;

};

