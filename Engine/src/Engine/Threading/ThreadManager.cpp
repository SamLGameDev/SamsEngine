#include "ThreadManager.h"
#include <thread>

void UThreadManager::DispatchJob(EThreadTypes thread)
{
}

void UThreadManager::InitialiseAllThreads()
{
	GameThread = std::thread();

	RenderThread = std::thread();

	WorkerThreads = Array<std::thread>(std::thread::hardware_concurrency() - 3);

	DispatchJob<EThreadTypes::RenderThread>();

	DispatchJob(EThreadTypes::GameThread);

}
