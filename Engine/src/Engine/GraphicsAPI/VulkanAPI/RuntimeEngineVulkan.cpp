#include "RuntimeEngineVulkan.h"
#include "Camera.h"
#include "VulkanInstance.h"
#include "VulkanLogicalDevice.h"
#include "VulkanRenderer.h"

namespace Vulkan
{

	ErrorCodes RuntimeEngine::Init()
	{
		SubsystemManager = new SubsystemInitialiser;

		ErrorCodes status = SubsystemManager->Init();

		if (status == ERROR)
		{
			return ERROR;
		}

		TimeLastFrame = glfwGetTime();

		return SUCCEEDED;
	}

	ErrorCodes RuntimeEngine::Loop()
	{
		const double time = glfwGetTime();
		const double deltaTime = time - TimeLastFrame;
		TimeLastFrame = time;
		Object::TickDel.Broadcast(deltaTime);
		SInstance::GetInstance()->GraphicsCard->GetRenderer()->Render();

		return SUCCEEDED;
	}

	ErrorCodes RuntimeEngine::ShutDown()
	{
		vkDeviceWaitIdle(*SInstance::GetInstance()->GraphicsCard->GetLogicalDevice()->GetVulkanLogicalDevice());

		SubsystemManager->ShutDown();

		delete SubsystemManager;

		Object::TickDel.RemoveAll();

		Model::LoadedTextures.Empty();

		glfwTerminate();

		return SUCCEEDED;
	}

	bool RuntimeEngine::ShouldClose()
	{
		return glfwWindowShouldClose(Camera::GetActiveWindow()->GetWindow());
	}

	void RuntimeEngine::WaitForFrameToFinish()
	{
		SInstance::GetInstance()->GraphicsCard->GetRenderer()->WaitForDrawToFinish();
	}

}
