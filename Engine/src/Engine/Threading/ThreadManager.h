#pragma once
#include <thread>

#include "Array.h"

enum EThreadTypes : std::uint8_t
{
	GameThread,
	RenderThread,
	WorkerThread
};


template<EThreadTypes RequestedThread, EThreadTypes RequiredThread>
concept MatchesThread = RequestedThread == RequiredThread;


class UThreadManager final
{

public:


	template<EThreadTypes T> requires MatchesThread<T, EThreadTypes::GameThread>
	void DispatchJob();

	template<EThreadTypes T> requires MatchesThread<T, EThreadTypes::RenderThread>
	void DispatchJob();

	template<EThreadTypes T> requires MatchesThread<T, EThreadTypes::WorkerThread>
	void DispatchJob();

	void DispatchJob(EThreadTypes thread);


protected:

	void InitialiseAllThreads();

	std::thread GameThread;

	std::thread RenderThread;

	Array<std::thread> WorkerThreads;


};

template <EThreadTypes T> requires MatchesThread<T, EThreadTypes::GameThread>
void UThreadManager::DispatchJob()
{
}

template <EThreadTypes T> requires MatchesThread<T, EThreadTypes::RenderThread>
void UThreadManager::DispatchJob()
{
}

template <EThreadTypes T> requires MatchesThread<T, EThreadTypes::WorkerThread>
void UThreadManager::DispatchJob()
{
}


