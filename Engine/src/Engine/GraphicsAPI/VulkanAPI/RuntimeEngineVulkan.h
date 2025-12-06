
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
