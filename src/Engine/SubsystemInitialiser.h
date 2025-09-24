
#pragma once

#include "ErrorCodes.h"
#include "FirstWindow.h"
#include "InputManager.h"
#include "ObjectFactory.h"
#include "World.h"

class SubsystemInitialiser
{
public:

	ErrorCodes Init();

private:

	FirstWindow Window;

	InputManager inputManager;

	World world;
};
