// DO NOT MARK
//This is because it has been submitted for my COMP305. Link to Original: https://github.falmouth.ac.uk/GA-Undergrad-Student-Work-25-26/Comp305-Engine-SL295211.git
#include "VulkanInstance.h"

#include <exception>
#include <iostream>
#include "WorldObject.h"
#include "ObjectFactory.h"
#include "UVulkanComputeShader.h"
namespace Vulkan
{

	std::optional<SInstance*> SInstance::Instance;

	
	ErrorCodes SInstance::ShutDown()
	{
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
