
#pragma once

#include "ErrorCodes.h"
#include "FirstWindow.h"
#include "InputManager.h"

class SubsystemInitialiser
{
public:

	ErrorCodes Init();

private:

	FirstWindow Window;

	InputManager inputManager;

};
