#include "ThreadManager.h"
#include <thread>

#include "WorkerThread.h"

std::unique_ptr<UThreadManager> UThreadManager::Instance = nullptr;

UThreadManager::UThreadManager()
{
	if (Instance)
	{
		return;
	}
	InitialiseAllThreads();
}

void UThreadManager::DispatchJob(EThreadTypes thread, const std::function<void()>& Job)
{
	switch (thread) 
	{
		case EThreadTypes::GameThread:
			DispatchJob<EThreadTypes::GameThread>(Job);
			break;
		case EThreadTypes::RenderThread:
			DispatchJob<EThreadTypes::RenderThread>(Job);
			break;
		case EThreadTypes::WorkerThread:
			DispatchJob<EThreadTypes::WorkerThread>(Job);
			break;
	}
}

UThreadManager* UThreadManager::Get()
{
	if (!Instance)
	{
		Instance = std::make_unique<UThreadManager>();
	}
	return Instance.get();
}

void UThreadManager::InitialiseAllThreads()
{
	GameThread = UGameThread();

	RenderThread = URenderThread();

	WorkerThreads = Array<UWorkerThread>(std::thread::hardware_concurrency() - 3);

}
