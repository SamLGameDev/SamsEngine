
#pragma once

#include "ErrorCodes.h"
#include "SubsystemInitialiser.h"

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
