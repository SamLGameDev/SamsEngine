#include "RenderThread.h"

#include "BaseRuntimeEngine.h"
#include "RuntimeEngine.h"
#include "RuntimeEngineVulkan.h"
#include "ThreadManager.h"


URenderThread::URenderThread(const EGraphicsAPI API)
{
	Thread = std::thread(&URenderThread::Run, this, API);
}

bool URenderThread::ShouldClose()
{
	return Vulkan::RuntimeEngine::ShouldClose();
}

void URenderThread::Run(const EGraphicsAPI API)
{
	BaseRuntimeEngine* engine = nullptr;

	switch (API)
	{
		case (EGraphicsAPI::Vulkan):
		{
			engine = new Vulkan::RuntimeEngine();
		}
		case (EGraphicsAPI::OpenGL):
		{
			engine = new OpenGL::RuntimeEngine();
		}
	}

	engine->Init();

	while (true)
	{
		if (UThreadManager::DoesTheadHaveQueuedJobs<RenderThread>())
		{
			
			auto job = UThreadManager::GetJob<RenderThread>();
			job();
			

		}
	}
}
