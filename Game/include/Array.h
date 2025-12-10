#pragma once
#include <initializer_list>
#include <utility>
#include <stdexcept>

#include "vulkan/vulkan_core.h"

template<typename T>
class Array
{
public:
	Array()
	{
		// TODO Need to separate numItems and ArraySize, but problem is vulkan stuff accesses the array directly, so size won't be updated
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

	Array(Array&& Other) noexcept
	{
		Move(Other);
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


	explicit Array(const size_t& Size)
	{
		NumItems = Size;
		ArraySize = Size;
		DynamicArray = new T[Size];
	}

	explicit Array(T* First, T* Last)
	{
		NumItems = 0;
		ArraySize = 0;
		DynamicArray = new T[1];
		for (T* i = First; i != Last; ++i)
		{
			Add(*i);
		}
	}

	Array& operator=(const Array& other)
	{
		if (this != &other)
		{
			Copy(other);
		}
		return *this;
	}

	void Move(Array& other)
	{
		delete[] DynamicArray;
		DynamicArray = other.DynamicArray;
		NumItems = other.NumItems;
		ArraySize = other.ArraySize;
		other.DynamicArray = nullptr;
		other.NumItems = 0;
		other.ArraySize = 0;
	}

	Array& operator=(Array&& other) noexcept
	{
		if (this != &other)
		{
			Move(other);
		}
		return *this;
	}



	bool operator==(const Array& other) const
	{
		if (other.GetSize() != NumItems)
		{
			return false;
		}

		for (size_t i = 0; i < other.GetSize(); i++)
		{
			if (other[i] != DynamicArray[i])
			{
				return false;
			}
		}
		return true;

	}

	bool operator==(const nullptr_t null)
	{
		return this == null;
	}

	[[nodiscard]] const T& operator[](const size_t& Index) const
	{
		return GetItemAtRef(Index);
	}
	[[nodiscard]] T& operator[](const size_t& Index)
	{
		return GetItemAtRef(Index);
	}


	[[nodiscard]] T& GetItemAtRef(const size_t& Index) const
	{
		return DynamicArray[Index];
	}

	[[nodiscard]] T* GetItemAtPtr(const size_t Index) const
	{
		return &DynamicArray[Index];
	}

	[[nodiscard]] T* GetFirstPtr()const
	{
		return DynamicArray;
	}

	[[nodiscard]] T* GetLastPtr() const
	{
		return &DynamicArray[NumItems - 1];
	}

	void Add(const T& Item)
	{
		//Depreciated for now, as arraySize needs to be seperated from NumItems properly
		if (NumItems + 1 < ArraySize)
		{
			NumItems++;
			DynamicArray[NumItems] = Item;
			return;
		}

		T* NewArray = new T[NumItems + 1];

		for (size_t i = 0; i < NumItems; i++)
		{
			NewArray[i] = std::move(GetItemAt(i));
		}
		NewArray[NumItems] = Item;
		NumItems++;

		delete[] DynamicArray;
		DynamicArray = NewArray;

		ArraySize = NumItems;
	}

	void Add(const Array& Item)
	{
		//Depreciated for now, as arraySize needs to be seperated from NumItems properly
		if (ArraySize > NumItems + Item.GetSize())
		{
			for (size_t i = 0; i < Item.GetSize(); i++)
			{
				DynamicArray[i + NumItems] = std::move(Item.GetItemAt(i));;
			}
			NumItems += Item.GetSize();
			return;
		}

		T* NewArray = new T[NumItems + Item.GetSize()];

		for (size_t i = 0; i < NumItems; i++)
		{
			NewArray[i] = std::move(GetItemAt(i));
		}

		for (size_t i = 0; i < Item.GetSize(); i++)
		{
			NewArray[i + NumItems] = std::move(Item.GetItemAt(i));
		}

		NumItems += Item.GetSize();;

		delete[] DynamicArray;
		DynamicArray = NewArray;

		ArraySize = NumItems;
	}

	/// <summary>
	/// Gets a copy of the item at the specified index
	/// </summary>
	/// <param name="Index"></param>
	/// <returns></returns>
	[[nodiscard]] T GetItemAt(const size_t& Index) const
	{
		return DynamicArray[Index];
	}

	[[nodiscard]] size_t GetSize() const
	{
		return NumItems;
	}

	//Get the first item in the underlying array
	[[nodiscard]] T* GetArray()
	{
		return DynamicArray;
	}

	bool Reallocate(const size_t& Size)
	{
		if (Size < ArraySize)
		{
			return false;
		}

		T* NewArray = new T[Size];

		for (size_t i = 0; i < NumItems; i++)
		{
			NewArray[i] = GetItemAt(i);
		}

		ArraySize = Size;
		NumItems = Size;

		delete[] DynamicArray;
		DynamicArray = NewArray;

		return true;
	}

	/// <summary>
	/// Does the Item exist in the array, and if so, updates Index to the location
	/// </summary>
	/// <param name="Item"></param>
	/// <param name="Index"></param>
	/// <returns></returns>
	bool Contains(const T& Item, size_t& Index) const
	{
		for (size_t i = 0; i < NumItems; i++)
		{
			if (DynamicArray[i] == Item)
			{
				Index = i;
				return true;
			}
		}
		return false;
	}

	bool Contains(const T& Item) const
	{
		for (size_t i = 0; i < NumItems; i++)
		{
			if (DynamicArray[i] == Item)
			{
				return true;
			}
		}
		return false;
	}

	bool Replace(const T& ToReplace, const T& NewItem)
	{
		for (size_t i = 0; i < NumItems; i++)
		{
			if (DynamicArray[i] == ToReplace)
			{
				DynamicArray[i] = NewItem;
				return true;
			}
		}
		return false;
	}

	bool ReplaceAt(const size_t& Index, const T& Item)
	{
		if (NumItems > Index)
		{
			DynamicArray[Index] = Item;
			return true;
		}
		return false;
	}

	void Remove(const T& Item)
	{
		size_t index;

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

	/// <summary>
	/// Empties the array
	/// </summary>
	void Empty()
	{
		delete[] DynamicArray;

		ArraySize = 0;
		NumItems = 0;
		DynamicArray = new T[1];
	}

	[[nodiscard]] bool IsEmpty() const
	{
		return NumItems == 0;
	}

	void Swap(const size_t& From, const size_t& To)
	{
		T temp = std::move(DynamicArray[From]);
		DynamicArray[From] = std::move(DynamicArray[To]);
		DynamicArray[To] = std::move(temp);
	}

	T* begin() { return DynamicArray; }
	T* end() { return DynamicArray + NumItems; }
	const T* begin() const { return DynamicArray; }
	const T* end() const { return DynamicArray + NumItems; }

private:

	void Copy(const Array& other)
	{
		if (other.GetSize() == 0)
		{
			delete[] DynamicArray;
			DynamicArray = new T[1];
			NumItems = 0;
			ArraySize = 0;
			return;
		}

		delete[] DynamicArray;
		DynamicArray = new T[other.GetSize()];
		for (size_t i = 0; i < other.GetSize(); i++)
		{
			DynamicArray[i] = std::move(other.GetItemAt(i));
		}
		NumItems = other.GetSize();
		ArraySize = other.ArraySize;
	}

	T* DynamicArray = nullptr;

	size_t NumItems;

	size_t ArraySize;
};
