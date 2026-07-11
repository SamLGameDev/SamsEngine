#pragma once

#include <cassert>
#include <memory>
#include <cmath>
#include <iostream>
#include <mutex>
#include <string>

#include "Array.h"
#include "MathCore.h"
#include "Asserts.h"


struct FQueueThreadPos
{
	size_t Head;
	size_t Tail;
};


template<typename T>
class TLFQueue
{
public:

	TLFQueue()
	{
		QueueContainer = new T*[InitialContainers];
		QueueContainer[0] = new T[InitialContainerSize];
		HeadIndex.exchange(0);
		TailIndex.exchange(0);
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
		NumItems.exchange(other.NumItems.load());
		AllocatedSize = other.AllocatedSize;
		return *this;
	}

	[[nodiscard]] T& operator[](const size_t Index) const
	{
		return GetItemAtRef(Index);
	}

	[[nodiscard]] T& GetItemAtRef(const size_t Index) const
	{
		CheckF(MathCore::WithinRange<size_t>(HeadIndex.load(), TailIndex.load(), Index), "Index out of bounds, Index: {}, NumItems: {}", Index, NumItems.load());

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
		std::scoped_lock guard(PushPopMtx);
		MoveTailAndReallocatedIfNeeded(1);

		size_t tmpTail = ReservedTail.exchange(ReservedTail.load() + 1 & AllocatedSize - 1);



		GetItemAtRefUnchecked(tmpTail ) = Item;

		while (tmpTail > TailIndex.load())
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(1));
		}

		TailIndex.exchange(tmpTail + 1 & AllocatedSize - 1);
		ThreadsCurrentlyAccessingMemory.fetch_add(-1);
	}


	bool IsAssignedMemory(const size_t Index)
	{
		if (TailIndex.load() < HeadIndex.load())
		{
			return Index < TailIndex.load() || Index > HeadIndex.load();
		}
		else
		{
			return Index < TailIndex.load() && Index >= HeadIndex.load();
		}
	}

	T Pop()
	{
		std::scoped_lock wait(PopWait);

		while (NumItems.load() < 1) std::this_thread::yield();

		std::scoped_lock guard(PushPopMtx);

		const T hea = GetItemAtRef(HeadIndex);

		if (!hea)
		{
			std::cout << "wh";
		}

		const T head = std::move(GetItemAtRef(HeadIndex));

		HeadIndex.exchange(HeadIndex + 1 & AllocatedSize - 1);
		NumItems.fetch_add(-1); //Race condition here
		return head;
	}

	bool IsEmpty() const
	{
		return NumItems.load() == 0;
	}


	void DebugLog()
	{
		std::cout << "Tail Index: " << TailIndex << "\n"
		<< "Head Index: " << HeadIndex << "\n"
		<< "NumItems: " << NumItems << "\n"
		<< "AllocatedSize: " << AllocatedSize << "\n";
	}

protected:

	T** QueueContainer;

	std::mutex PushPopMtx;

	std::mutex PopWait;
	//Tail is last element, not first empty element
	std::atomic<size_t> TailIndex = 0;
	std::atomic<size_t> HeadIndex = 0;

	std::atomic<size_t> ReservedTail = 0;

	std::atomic<size_t> ReservedHead = 0;

	std::atomic<size_t> NumItems = 0;

	std::atomic<size_t> ThreadsCurrentlyAccessingMemory;

	size_t AllocatedSize = InitialContainerSize;

	size_t AllocatedContainers = InitialContainers;	

	std::mutex printm;

	Array<FQueueThreadPos> ThreadPositions;

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

	[[nodiscard]] size_t GetRemainingSpaceInContainer(const size_t Index) const
	{
		return GetIndexInContainer(Index);
	}

	[[nodiscard]] size_t GetNumItemsInCurrentContainer() const
	{
		return GetIndexInContainer(NumItems);
	}

	[[nodiscard]] size_t GetNumItemsInCurrentContainer(const size_t NumItemsAtFull) const
	{
		return GetIndexInContainer(NumItemsAtFull);
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

	[[nodiscard]] size_t GetNumItemsInContainer(const size_t Container, const size_t NumItemsAtFull) const
	{
		const size_t currentContainer = GetContainerIndex(NumItemsAtFull);

		if (Container == currentContainer) return GetNumItemsInCurrentContainer(NumItemsAtFull);

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

	void AllocateNeededSlots(const size_t NumSlots, const size_t NumItemsAtFull)
	{

		const size_t normalGrowth = AllocatedSize * QueueGrowthMultiplier;

		size_t NewContainerSize = AllocatedSize + NumSlots < normalGrowth ? normalGrowth : std::bit_ceil((AllocatedSize + NumSlots) * QueueGrowthMultiplier);
		float AdditionalSlots = static_cast<float>(NewContainerSize) - static_cast<float>(AllocatedSize);

		// calculates how many new unallocated containers are needed,
		// taking the needed container amount and subtracting the unused but allocated containers
		const size_t NewContainersNeeded = (std::max<float>(0, AdditionalSlots - GetRemainingSpaceInContainer(NumItemsAtFull)) / MaxItemsPerContainer);


		T** NewQueue = new T*[AllocatedContainers + NewContainersNeeded];

		size_t currentContainer = 0;

		const size_t tmpAllocatedSize = NewContainerSize;

		while (NewContainerSize > 0)
		{
			const size_t newSlotsNum = std::min<float>(NewContainerSize, MaxItemsPerContainer);
			NewQueue[currentContainer] = new T[newSlotsNum];
			NewContainerSize -= newSlotsNum;
			++currentContainer;
		}

		if (!IsEmpty()) {

			if (HeadIndex.load() < TailIndex.load())
			{
				currentContainer = 0;
				for (size_t i = GetContainerIndex(HeadIndex); i <= GetContainerIndex(TailIndex); ++i)
				{
					std::move(QueueContainer[i], QueueContainer[i] + GetNumItemsInContainer(i, NumItemsAtFull), NewQueue[currentContainer]);
					currentContainer++;
				}
			}
			else
			{
				currentContainer = 0;
			// Correctly move elements from the wrapped queue into the newly allocated containers.
			// Use atomic loads for head/tail indices and copy in two phases: [headContainer..end] then [0..tailContainer].
			currentContainer = 0;
			auto headIdxAtomic = HeadIndex.load();
			auto tailIdxAtomic = TailIndex.load();
			size_t headContainer = GetContainerIndex(headIdxAtomic);
			size_t tailContainer = GetContainerIndex(tailIdxAtomic);
			size_t headIdx = GetIndexInContainer(headIdxAtomic);
			size_t tailIdx = GetIndexInContainer(tailIdxAtomic);

			// helper to compute new container sizes based on the total new allocation (tmpAllocatedSize)
			auto GetNewContainerSize = [&](size_t idx) -> size_t {
				size_t full = tmpAllocatedSize / MaxItemsPerContainer;
				size_t rem = tmpAllocatedSize % MaxItemsPerContainer;
				if (idx < full) return MaxItemsPerContainer;
				if (idx == full) return rem;
				return 0;
			};

			// destination offset within current new container
			size_t destOffset = 0;

			auto move_into_dest = [&](T* src, size_t count) {
				size_t srcOff = 0;
				while (count > 0) {
					size_t destCap = GetNewContainerSize(currentContainer);
					// if destCap is zero something is wrong; guard against division by zero
					if (destCap == 0) return;
					size_t room = destCap - destOffset;
					size_t toMove = std::min(count, room);
					std::move(src + srcOff, src + srcOff + toMove, NewQueue[currentContainer] + destOffset);
					srcOff += toMove;
					count -= toMove;
					destOffset += toMove;
					if (destOffset == destCap) { currentContainer++; destOffset = 0; }
				}
			};

			// Phase 1: from headContainer to end of allocated containers
			for (size_t i = headContainer; i < AllocatedContainers; ++i) {
				size_t start = (i == headContainer) ? headIdx : 0;
				size_t endExclusive = GetNumItemsInContainer(i, NumItemsAtFull);
				if (endExclusive > start) {
					move_into_dest(QueueContainer[i] + start, endExclusive - start);
				}
			}

			// Phase 2: from container 0 up to tailContainer
			for (size_t i = 0; i <= tailContainer; ++i) {
				size_t start = 0;
				size_t endExclusive = (i == tailContainer) ? tailIdx : GetNumItemsInContainer(i, NumItemsAtFull);
				if (endExclusive > start) {
					move_into_dest(QueueContainer[i] + start, endExclusive - start);
				}
			}
			}
		}

		DeleteCurrentQueue();
		HeadIndex = 0;
		TailIndex = NumItemsAtFull;
		ReservedTail = NumItemsAtFull;
		ReservedHead = 0;
		QueueContainer = NewQueue;
		AllocatedContainers = AllocatedContainers + NewContainersNeeded;
		AllocatedSize = tmpAllocatedSize;

		for (size_t i = 0; i < NumItemsAtFull; i++)
		{
			if (!GetItemAtRef(i))
			{
				std::cout << "Bad Move";
			}
		}
	}

	void MoveTail(const size_t NumSpaces)
	{
		if (NumItems == 0)
		{
			TailIndex = 0;
		}
		else
		{
			TailIndex = TailIndex + NumSpaces & AllocatedSize - 1;
		}
	}

	void MoveTailAndReallocatedIfNeeded(const size_t NumSpaces)
	{
		const size_t tmpNumItems = NumItems.fetch_add(NumSpaces);

		while (tmpNumItems > AllocatedSize)
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(1));
		}



		if (tmpNumItems + NumSpaces > AllocatedSize)
		{

			while (ThreadsCurrentlyAccessingMemory.load() > 0)
			{
				std::this_thread::sleep_for(std::chrono::milliseconds(1));
			}

			AllocateNeededSlots(NumSpaces, tmpNumItems);
		}
		ThreadsCurrentlyAccessingMemory.fetch_add(1);


	}

	[[nodiscard]] T& GetItemAtRefUnchecked(const size_t Index) const
	{
		return QueueContainer[GetContainerIndex(Index)][GetIndexInContainer(Index)];
	}

};
