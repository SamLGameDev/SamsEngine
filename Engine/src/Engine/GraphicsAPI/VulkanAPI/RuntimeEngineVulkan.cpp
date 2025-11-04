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

		SInstance::GetInstance()->GraphicsCard->GetRenderer()->Render();

		return SUCCEEDED;
	}

	ErrorCodes RuntimeEngine::ShutDown()
	{
		vkDeviceWaitIdle(*SInstance::GetInstance()->GraphicsCard->GetLogicalDevice()->GetVulkanLogicalDevice());

		SubsystemManager->ShutDown();

		delete SubsystemManager;

		glfwTerminate();

		return SUCCEEDED;
	}

	bool RuntimeEngine::ShouldClose()
	{
		return glfwWindowShouldClose(Camera::GetActiveWindow()->GetWindow());
	}
}
