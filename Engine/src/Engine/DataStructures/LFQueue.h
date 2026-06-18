#pragma once

#include <cassert>
#include <memory>
#include <cmath>
#include <iostream>

template<typename T>
class TLFQueue
{
public:

	TLFQueue()
	{
		QueueContainer = new T*[InitialContainers];
		QueueContainer[0] = new T[InitialContainerSize];;
	}


	~TLFQueue()
	{
		for (size_t i = 0; i < GetNumContainers(); ++i)
		{
			delete[] QueueContainer[i];
		}
		delete[] QueueContainer;
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
		assert(Index < NumItems &&"Index is out of bounds");

		return QueueContainer[GetContainerIndex(Index)][GetIndexInContainer(Index)];
	}


	[[nodiscard]] T& GetHead() const
	{
		return QueueContainer[0][0];
	}

	[[nodiscard]] T& GetTail() const
	{
		return QueueContainer[GetContainerIndex(NumItems)][GetIndexInContainer(NumItems)];
	}

	void Add(const T& Item)
	{
		GetTail() = Item;
		NumItems++;

		if (NumItems == AllocatedSize)
		{
			const size_t NewContainerSize = AllocatedSize * QueueGrowthMultiplier;
			float AdditionalSlots = static_cast<float>(NewContainerSize) - static_cast<float>(AllocatedSize);


			std::cout << GetRemainingSpaceInCurrentContainer() << std::endl;

			if (GetRemainingSpaceInCurrentContainer() > 0)
			{

				const size_t newSlotsNum = std::min<float>(AdditionalSlots, GetRemainingSpaceInCurrentContainer());

				AllocateAdditionalContainerSlots(newSlotsNum, GetContainerIndex(NumItems));
				
				AdditionalSlots = std::max<float>(0, AdditionalSlots - GetRemainingSpaceInCurrentContainer());
				
			}


			if (AdditionalSlots <= 0) return;

			// calculates how many new unallocated containers are needed,
			// taking the needed container amount and subtracting the unused but allocated containers
			const size_t NewContainersNeeded = (AdditionalSlots / MaxItemsPerContainer);
			const float newAllocationsNeeded = NewContainersNeeded - ((AllocatedContainers - 1) - GetContainerIndex(NumItems));

			if (newAllocationsNeeded > 0) AddNewContainer(std::ceil(newAllocationsNeeded));

			size_t currentContainer = GetContainerIndex(NumItems);

			while (AdditionalSlots > 0)
			{
				const size_t newSlotsNum = std::min<float>(AdditionalSlots, MaxItemsPerContainer);
				AllocateAdditionalContainerSlots(newSlotsNum, currentContainer);
				AdditionalSlots -= newSlotsNum;
				++currentContainer;
			}
		}
	}


	T Pop()
	{

		assert(NumItems > 0, "Queue is empty");

		T head = std::move(GetHead());

		for (size_t i = 0; i < AllocatedContainers; i++)
		{ 
			std::move(QueueContainer[i] + 1, QueueContainer[i] + GetNumItemsInContainer(i), QueueContainer[i]);
			
			if (i < AllocatedContainers - 1)
			{
				QueueContainer[i][GetNumItemsInContainer(i) - 1] = QueueContainer[i + 1][0];
			}
		}

		NumItems--;

		return head;
	}

protected:

	T** QueueContainer;

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

		assert(Amount > 0 && "Amount must be greater than 0");

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

		assert(Amount > 0 && "Amount must be greater than 0");

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

};
