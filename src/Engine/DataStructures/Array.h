#pragma once
#include <initializer_list>
#include <utility>
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
		Copy(CopyArray);
	}

	Array(std::initializer_list<T> Init)
	{
		NumItems = Init.size();
		ArraySize = Init.size();

		T* newArray = new T[NumItems];

		unsigned int index = 0;
		for (const T item : Init)
		{
			newArray[index++] = item;
		}

		delete[] DynamicArray;
		DynamicArray = newArray;
	}

	Array& operator=(const Array& other)
	{
		if (this != &other)
		{
			Copy(other);
		}
		return *this;
	}

	[[nodiscard]] T operator[](const unsigned int Index) const
	{
		return GetItemAt(Index);
	}

	[[nodiscard]] T* GetItemAtRef(const unsigned int Index) const
	{
		return &DynamicArray[Index];
	}

	[[nodiscard]] T* GetFirstRef()const
	{
		return DynamicArray;
	}

	void Add(const T item)
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

	void Add(const Array& item)
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

	[[nodiscard]] T GetItemAt(unsigned int Index) const
	{
		return DynamicArray[Index];
	}

	[[nodiscard]] unsigned int GetSize() const
	{
		return NumItems;
	}

	[[nodiscard]] T GetArray()
	{
		return DynamicArray;
	}

	bool Reallocate(const unsigned int Size)
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

	bool Contains(const T& Item, unsigned int& Index) const
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

	void Remove(const T& Item)
	{
		unsigned int index;

		if (Contains(Item, index))
		{
			T* NewArray = new T[ArraySize - 1];
			unsigned int Skip = 0;
			for (unsigned int i = 0; i < NumItems - 1; i++)
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

	[[nodiscard]] bool IsEmpty() const
	{
		return NumItems == 0;
	}

	T* begin() { return DynamicArray; }
	T* end() { return DynamicArray + NumItems; }
	const T* begin() const { return DynamicArray; }
	const T* end() const { return DynamicArray + NumItems; }

private:

	void Copy(const Array& other)
	{
		delete[] DynamicArray;
		DynamicArray = new T[other.GetSize()];
		for (unsigned int i = 0; i < other.GetSize(); i++)
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
