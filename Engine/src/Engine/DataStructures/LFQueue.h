#pragma once

#include <cassert>
#include <memory>
#include <cmath>
#include <iostream>

#include "MathCore.h"
#include "Asserts.h"

template<typename T>
class TLFQueue
{
public:

	TLFQueue()
	{
		QueueContainer = new T*[InitialContainers];
		QueueContainer[0] = new T[InitialContainerSize];
		HeadIndex = 0;
		TailIndex = 0;
	}

	~TLFQueue()
	{
		DeleteCurrentQueue();
	}

	TLFQueue(const TLFQueue& other)
		: QueueContainer(other.QueueContainer),
		  NumItems(other.NumItems),
		  AllocatedSize(other.AllocatedSize),
		  MaxItemsPerContainer(other.MaxItemsPerContainer)
	{
	}

	TLFQueue(TLFQueue&& other) noexcept
		: QueueContainer(other.QueueContainer),
		  NumItems(other.NumItems),
		  AllocatedSize(other.AllocatedSize),
		  MaxItemsPerContainer(other.MaxItemsPerContainer)
	{
	}

	TLFQueue& operator=(const TLFQueue& other)
	{
		if (this == &other)
			return *this;
		QueueContainer = other.QueueContainer;
		NumItems = other.NumItems;
		AllocatedSize = other.AllocatedSize;
		MaxItemsPerContainer = other.MaxItemsPerContainer;
		return *this;
	}

	TLFQueue& operator=(TLFQueue&& other) noexcept
	{
		if (this == &other)
			return *this;
		QueueContainer = other.QueueContainer;
		NumItems = other.NumItems;
		AllocatedSize = other.AllocatedSize;
		MaxItemsPerContainer = other.MaxItemsPerContainer;
		return *this;
	}

	[[nodiscard]] T& operator[](const size_t Index) const
	{
		return GetItemAtRef(Index);
	}

	[[nodiscard]] T& GetItemAtRef(const size_t Index) const
	{
		CheckF(MathCore::WithinRange(HeadIndex, TailIndex, Index), "Index out of bounds, Index: {}, NumItems: {}", Index, NumItems);

		return QueueContainer[GetContainerIndex(Index)][GetIndexInContainer(Index)];
	}


	[[nodiscard]] T& GetHead() const
	{
		return GetItemAtRef(HeadIndex);
	}

	[[nodiscard]] T& GetTail() const
	{
		return GetItemAtRef(TailIndex);
	}



	void Add(const T& Item)
	{
		MoveTailAndReallocatedIfNeeded(1);

		GetItemAtRefUnchecked(TailIndex) = Item;
		NumItems++;
	}


	T Pop()
	{

		CheckF(NumItems > 0, "Queue is empty");

		const T head = std::move(GetHead());

		NumItems--;
		HeadIndex++;

		return head;
	}

	bool IsEmpty() const
	{
		return NumItems == 0;
	}

protected:

	T** QueueContainer;
	//Tail is last element, not first empty element
	size_t TailIndex = 0;
	size_t HeadIndex = 0;

	size_t NumItems = 0;

	size_t AllocatedSize = InitialContainerSize;

	size_t AllocatedContainers = InitialContainers;	

	constexpr static size_t MaxContainerSize = 1024;

	constexpr static size_t InitialContainerSize = 8;

	constexpr static size_t QueueGrowthMultiplier = 2;

	constexpr static size_t InitialContainers = 1;

	const size_t MaxItemsPerContainer = MaxContainerSize / sizeof(T);

	[[nodiscard]] size_t GetNumContainers() const
	{
		return AllocatedSize / MaxItemsPerContainer;
	}

	[[nodiscard]] size_t GetRemainingSpaceInCurrentContainer() const
	{
		return GetIndexInContainer(NumItems);
	}

	[[nodiscard]] size_t GetNumItemsInCurrentContainer() const
	{
		return GetIndexInContainer(NumItems);
	}

	[[nodiscard]] size_t GetContainerIndex(const size_t Index) const
	{
		return Index / MaxItemsPerContainer;
	}

	[[nodiscard]] size_t GetIndexInContainer(const size_t Index) const
	{
		return Index % MaxItemsPerContainer;
	}

	[[nodiscard]] size_t GetNumItemsInContainer(const size_t Container) const
	{
		const size_t currentContainer = GetContainerIndex(NumItems);

		if (Container == currentContainer) return GetNumItemsInCurrentContainer();
		
		if (Container < currentContainer) return MaxItemsPerContainer;

		return 0;
	}

	bool IsCurrentContainerFull() const
	{
		return GetIndexInContainer(NumItems) * sizeof(T) >= MaxContainerSize;
	}

	void AddNewContainer()
	{
		T** NewQueue = new T*[AllocatedContainers + 1];
		std::move(QueueContainer, QueueContainer + AllocatedContainers, NewQueue);
		delete[] QueueContainer;
		QueueContainer = NewQueue;

		QueueContainer[AllocatedContainers] = new T[InitialContainerSize];
		AllocatedContainers++;

	}

	void AddNewContainer(const size_t Amount)
	{

		CheckF(Amount > 0, "Amount must be greater than 0");

		T** NewQueue = new T*[AllocatedContainers + Amount];
		std::move(QueueContainer, QueueContainer + AllocatedContainers, NewQueue);
		delete[] QueueContainer;
		QueueContainer = NewQueue;

		for (size_t i = 0; i < Amount; ++i)
		{
			QueueContainer[AllocatedContainers + i] = new T[InitialContainerSize];
		}
		AllocatedContainers += Amount;
		
	}

	void AllocateAdditionalContainerSlots(const size_t Amount, const size_t Container)
	{

		CheckF(Amount > 0, "Amount must be greater than 0");

		const size_t currentContainerSize = GetNumItemsInContainer(Container);

		T* newContainer = new T[Amount + currentContainerSize];


		if (currentContainerSize != 0)
		{
			std::move(QueueContainer[Container],
				QueueContainer[Container] + currentContainerSize, newContainer);
		}

		delete[] QueueContainer[Container];

		QueueContainer[Container] = newContainer;

		AllocatedSize += Amount;
	}

	void DeleteCurrentQueue()
	{
		for (size_t i = 0; i < AllocatedContainers; ++i)
		{
			delete[] QueueContainer[i];
		}
		delete[] QueueContainer;
	}

	void AllocateNeededSlots(const size_t NumSlots)
	{

		const size_t normalGrowth = AllocatedSize * QueueGrowthMultiplier;

		size_t NewContainerSize = AllocatedSize + NumSlots < normalGrowth ? normalGrowth : std::bit_ceil((AllocatedSize + NumSlots) * QueueGrowthMultiplier);
		float AdditionalSlots = static_cast<float>(NewContainerSize) - static_cast<float>(AllocatedSize);

		// calculates how many new unallocated containers are needed,
		// taking the needed container amount and subtracting the unused but allocated containers
		const size_t NewContainersNeeded = (std::max<float>(0, AdditionalSlots - GetRemainingSpaceInCurrentContainer()) / MaxItemsPerContainer);


		T** NewQueue = new T*[AllocatedContainers + NewContainersNeeded];

		size_t currentContainer = 0;

		AllocatedSize = NewContainerSize;

		while (NewContainerSize > 0)
		{
			const size_t newSlotsNum = std::min<float>(NewContainerSize, MaxItemsPerContainer);
			NewQueue[currentContainer] = new T[newSlotsNum];
			NewContainerSize -= newSlotsNum;
			++currentContainer;
		}

		if (!IsEmpty()) {

			if (HeadIndex < TailIndex)
			{
				currentContainer = 0;
				for (size_t i = GetContainerIndex(HeadIndex); i <= GetContainerIndex(TailIndex); ++i)
				{
					std::move(QueueContainer[i], QueueContainer[i] + GetNumItemsInContainer(i), NewQueue[currentContainer]);
					currentContainer++;
				}
			}
			else
			{
				currentContainer = 0;
				size_t headIndex = GetIndexInContainer(HeadIndex);
				for (size_t i = GetContainerIndex(HeadIndex); i <= GetContainerIndex(AllocatedContainers); ++i)
				{

					std::move(QueueContainer[i] + headIndex, QueueContainer[i] + GetNumItemsInContainer(i), NewQueue[currentContainer]);
					currentContainer++;
					headIndex = 0;
				}

				for (size_t i = 0; i <= GetContainerIndex(HeadIndex); ++i)
				{
					const size_t tailIndex = i == HeadIndex ? GetIndexInContainer(TailIndex) : 0;
					std::move(QueueContainer[i], QueueContainer[i] + (tailIndex!=0 ? tailIndex : GetNumItemsInContainer(i)), NewQueue[currentContainer]);
					currentContainer++;
				}
			}
		}

		DeleteCurrentQueue();

		QueueContainer = NewQueue;
		AllocatedContainers = AllocatedContainers + NewContainersNeeded;
		HeadIndex = 0;
		TailIndex = NumItems - 1;
	}

	void MoveTail(const size_t NumSpaces)
	{
		if (NumItems == 0)
		{
			TailIndex = 0;
		}
		else
		{
			TailIndex = MathCore::WrapExclusive<size_t>(0, AllocatedSize, TailIndex + NumSpaces);
		}
	}

	void MoveTailAndReallocatedIfNeeded(const size_t NumSpaces)
	{
		if (NumItems + NumSpaces > AllocatedSize)
		{
			AllocateNeededSlots(NumSpaces);
		}

		MoveTail(NumSpaces);

	}

	[[nodiscard]] T& GetItemAtRefUnchecked(const size_t Index) const
	{
		return QueueContainer[GetContainerIndex(Index)][GetIndexInContainer(Index)];
	}

};
