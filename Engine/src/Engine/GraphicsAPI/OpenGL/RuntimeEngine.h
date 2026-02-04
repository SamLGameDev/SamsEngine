// DO NOT MARK.
//This is because it has been submitted for my dissertation. Link to Original: https://github.falmouth.ac.uk/GA-Undergrad-Student-Work-25-26/Dissertation-SL295211.git

#pragma once

#include "ErrorCodes.h"
#include "SubsystemInitialiser.h"

namespace OpenGL {

	class RuntimeEngine
	{
	public:

		RuntimeEngine() = default;

		ErrorCodes Init();

		ErrorCodes Loop();

		ErrorCodes ShutDown();

		static bool ShouldClose();

	private:

		SubsystemInitialiser* SubsystemManager;

		double TimeLastFrame;

		GLsync Gsync;

	};
}
