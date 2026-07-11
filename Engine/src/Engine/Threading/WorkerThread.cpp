#include "WorkerThread.h"

#include "ThreadManager.h"

UWorkerThread::UWorkerThread()
{
	Thread = std::thread(&UWorkerThread::Run, this);
}

void UWorkerThread::Run()
{
	while (true)
	{
		if (UThreadManager::DoesTheadHaveQueuedJobs<WorkerThread>())
		{

			auto job = UThreadManager::GetJob<WorkerThread>();
			job();
			

		}
	}
}
