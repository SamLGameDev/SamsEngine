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
	Array& operator=(const Array& other)
	{
		if (this != &other)
		{ 
			copy(other);
		}
		return *this;
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

	T GetItemAt(int Index)
	{
		return DynamicArray[Index];
	}

	const int GetSize() const
	{
		return ArraySize;
	}

	T GetArray()
	{
		return DynamicArray;
	}

private:

	void copy(Array other)
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

	int ArraySize;

};

