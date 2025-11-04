#include "VulkanInstance.h"

#include <exception>
#include <iostream>
#include "WorldObject.h"
#include "ObjectFactory.h"

namespace Vulkan
{

	std::optional<SInstance*> SInstance::Instance;

	
	ErrorCodes SInstance::ShutDown()
	{

		vkDestroySurfaceKHR(VulkanInstance, WindowsInterface, nullptr);

		vkDestroyInstance(VulkanInstance, nullptr);

		return SUCCEEDED;
	}

	SInstance* SInstance::GetInstance()
	{
		if (Instance.has_value())
		{
			return Instance.value();
		}

		Instance = new SInstance();

		return Instance.value();
	}
}
