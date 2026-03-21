// DO NOT MARK
//This is because it has been submitted for my COMP305. Link to Original: https://github.falmouth.ac.uk/GA-Undergrad-Student-Work-25-26/Comp305-Engine-SL295211.git

#pragma once

#include "ErrorCodes.h"
#include "VulkanSubsystemInitialiser.h"

namespace Vulkan
{

	class RuntimeEngine
	{
	public:

		RuntimeEngine() = default;

		ErrorCodes Init();

		ErrorCodes Loop();

		ErrorCodes ShutDown();

		static bool ShouldClose();

		static void WaitForFrameToFinish();

	private:

		SubsystemInitialiser* SubsystemManager;

		double TimeLastFrame;

	};
}
