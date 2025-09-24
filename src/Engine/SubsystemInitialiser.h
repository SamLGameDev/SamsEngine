
#pragma once

#include "ErrorCodes.h"
#include "FirstWindow.h"

class SubsystemInitialiser
{
public:

	ErrorCodes Init();

private:

	FirstWindow Window;

};
