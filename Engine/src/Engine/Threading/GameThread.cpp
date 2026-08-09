#include "GameThread.h"

#include "ThreadManager.h"

UGameThread::UGameThread()
{
	Thread = std::thread(&UGameThread::Run, this);
}

void UGameThread::Run()
{
	while (true)
	{
		if (UThreadManager::DoesTheadHaveQueuedJobs<GameThread>())
		{
			auto job = UThreadManager::GetJob<GameThread>();
			job();
		}
	}
}
