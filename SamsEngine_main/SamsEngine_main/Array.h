#pragma once
#include <iostream>
template<typename T>
class Array
{
public:
	Array()
	{
		NumItems = 0;
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

		if (NumItems + 1 < ArraySize)
		{
			NumItems++;
			DynamicArray[NumItems] = item;
			return;
		}
		T* NewArray = new T[NumItems + 1];

		for (int i = 0; i < NumItems; i++)
		{
			NewArray[i] = std::move(GetItemAt(i));;
		}
		NewArray[NumItems] = item;
		NumItems++;

		delete[] DynamicArray;
		DynamicArray = NewArray;

		ArraySize = NumItems;

	}


	void Add(Array<T>& item)
	{
		if (ArraySize > NumItems + item.GetSize())
		{
			for (unsigned int i = 0; i < item.GetSize(); i++)
			{
				DynamicArray[i + NumItems] = std::move(item.GetItemAt(i));;
			}
			NumItems += item.GetSize();
			return;
		}

		T* NewArray = new T[NumItems + item.GetSize()];

		for (unsigned int i = 0; i < NumItems; i++)
		{
			NewArray[i] = std::move(GetItemAt(i));;
		}

		for (unsigned int i = 0; i < item.GetSize(); i++)
		{
			NewArray[i + NumItems] = std::move(item.GetItemAt(i));;
		}

		NumItems += item.GetSize();;

		delete[] DynamicArray;
		DynamicArray = NewArray;

		ArraySize = NumItems;

	}

	T GetItemAt(int Index) const
	{
		return DynamicArray[Index];
	}

	const unsigned int GetSize() const
	{
		return NumItems;
	}

	T GetArray()
	{
		return DynamicArray;
	}

	const bool Contains(T& Item, unsigned int& Index)
	{
		for (unsigned int i = 0; i < NumItems; i++)
		{
			if (Item == DynamicArray[i])
			{
				Index = i;
				return true;
			}
		}
		return false;
	}

	const bool Reallocate(const unsigned int Size)
	{
		if (Size > ArraySize)
		{
			return false;
		}

		T* NewArray = new T[Size];

		for (unsigned int i = 0; i < NumItems; i++)
		{
			NewArray[i] = std::move(GetItemAt(i));
		}

		ArraySize = Size;

		delete[] DynamicArray;
		DynamicArray = NewArray;

		return true;

	}

	const bool Contains(const T& Item, unsigned int& Index) const
	{
		for (unsigned int i = 0; i < NumItems; i++)
		{
			if (DynamicArray[i] == Item)
			{
				Index = i;
				return true;
			}
		}
		return false;
	}

	void Remove(T& Item)
	{
		unsigned int index;

		if (Contains(Item, index))
		{
			T* NewArray = new T[ArraySize - 1];
			unsigned int Skip = 0;
			for (unsigned int i = 0; i < NumItems-1; i++)
			{
				if (i == index)
				{
					Skip = 1;
				}

				NewArray[i] = std::move(DynamicArray[i + Skip]);
			}
			delete[] DynamicArray;
			DynamicArray = NewArray;
			NumItems--;
			ArraySize--;

		}
	}

private:

	void copy(const Array& other)
	{
		delete[] DynamicArray;
		DynamicArray = new T[other.GetSize()];
		for (int i = 0; i < other.GetSize(); i++)
		{
			DynamicArray[i] = std::move(other.GetItemAt(i));
		}
		NumItems = other.GetSize();
		ArraySize = other.ArraySize;
	}

	T* DynamicArray = nullptr;

	unsigned int NumItems;

	unsigned int ArraySize;

};

