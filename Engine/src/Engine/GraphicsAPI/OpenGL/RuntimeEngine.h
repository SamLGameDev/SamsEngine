

#pragma once

#include "BaseRuntimeEngine.h"
#include "ErrorCodes.h"
#include "SubsystemInitialiser.h"

namespace OpenGL {

	class RuntimeEngine : public BaseRuntimeEngine
	{
	public:

		RuntimeEngine() = default;

		ErrorCodes Init() override;

		ErrorCodes Loop();

		ErrorCodes ShutDown();

		static bool ShouldClose();

	private:

		SubsystemInitialiser* SubsystemManager;

		double TimeLastFrame;

		GLsync Gsync;

	};
}
