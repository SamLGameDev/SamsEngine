#pragma once
#include <functional>
#include <thread>

#include "Array.h"
#include "LFQueue.h"
#include "WorkerThread.h"
#include <memory>

#include "GameThread.h"
#include "RenderThread.h"

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

	UThreadManager();


	template<EThreadTypes T> requires MatchesThread<T, EThreadTypes::GameThread>
	static void DispatchJob(const std::function<void()>& Job);

	template<EThreadTypes T> requires MatchesThread<T, EThreadTypes::RenderThread>
	static void DispatchJob(const std::function<void()>& Job);

	template<EThreadTypes T> requires MatchesThread<T, EThreadTypes::WorkerThread>
	static void DispatchJob(const std::function<void()>& Job);

	static void DispatchJob(EThreadTypes thread, const std::function<void()>& Job);

	template<EThreadTypes T> requires MatchesThread<T, EThreadTypes::WorkerThread>
	static bool DoesTheadHaveQueuedJobs();

	template<EThreadTypes T> requires MatchesThread<T, EThreadTypes::GameThread>
	static bool DoesTheadHaveQueuedJobs();

	template<EThreadTypes T> requires MatchesThread<T, EThreadTypes::RenderThread>
	static bool DoesTheadHaveQueuedJobs();

	template<EThreadTypes T> requires MatchesThread<T, EThreadTypes::WorkerThread>
	static std::function<void()> GetJob();

	template<EThreadTypes T> requires MatchesThread<T, EThreadTypes::GameThread>
	static std::function<void()> GetJob();

	template<EThreadTypes T> requires MatchesThread<T, EThreadTypes::RenderThread>
	static std::function<void()> GetJob();

	static UThreadManager* Get();


protected:

	void InitialiseAllThreads();

	UGameThread GameThread;

	URenderThread RenderThread;

	Array<UWorkerThread> WorkerThreads;

	TLFQueue<std::function<void()>> GameJobQueue;
	TLFQueue<std::function<void()>> RenderJobQueue;
	TLFQueue<std::function<void()>> WorkerJobQueue;

	static std::unique_ptr<UThreadManager> Instance;


};

template <EThreadTypes T> requires MatchesThread<T, EThreadTypes::GameThread>
void UThreadManager::DispatchJob(const std::function<void()>& Job)
{
	UThreadManager::Get()->GameJobQueue.Add(Job);
}

template <EThreadTypes T> requires MatchesThread<T, EThreadTypes::RenderThread>
void UThreadManager::DispatchJob(const std::function<void()>& Job)
{
	UThreadManager::Get()->RenderJobQueue.Add(Job);
}

template <EThreadTypes T> requires MatchesThread<T, EThreadTypes::WorkerThread>
void UThreadManager::DispatchJob(const std::function<void()>& Job)
{
	UThreadManager::Get()->WorkerJobQueue.Add(Job);
}

template <EThreadTypes T> requires MatchesThread<T, EThreadTypes::WorkerThread>
bool UThreadManager::DoesTheadHaveQueuedJobs()
{
	return !UThreadManager::Get()->WorkerJobQueue.IsEmpty();
}

template <EThreadTypes T> requires MatchesThread<T, EThreadTypes::GameThread>
bool UThreadManager::DoesTheadHaveQueuedJobs()
{
	return !UThreadManager::Get()->GameJobQueue.IsEmpty();
}

template <EThreadTypes T> requires MatchesThread<T, EThreadTypes::RenderThread>
bool UThreadManager::DoesTheadHaveQueuedJobs()
{
	return !UThreadManager::Get()->RenderJobQueue.IsEmpty();
}

template <EThreadTypes T> requires MatchesThread<T, EThreadTypes::WorkerThread>
std::function<void()> UThreadManager::GetJob()
{
	return Get()->WorkerJobQueue.Pop();
}

template <EThreadTypes T> requires MatchesThread<T, EThreadTypes::GameThread>
std::function<void()> UThreadManager::GetJob()
{
	return Get()->GameJobQueue.Pop();
}

template <EThreadTypes T> requires MatchesThread<T, EThreadTypes::RenderThread>
std::function<void()> UThreadManager::GetJob()
{
	return Get()->RenderJobQueue.Pop();
}


