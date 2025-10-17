
#pragma once

#include "ErrorCodes.h"
#include "FirstWindow.h"
#include "InputManager.h"

#include "World.h"
#include "Camera.h"
#include "Renderer.h"
#include "LightManager.h"
#include "CorePaths.h"
#include "GraphicsAPIConstructor.h"

#include <memory>

class SubsystemInitialiser
{
public:

	SubsystemInitialiser(){};

	ErrorCodes Init();

	ErrorCodes ShutDown();

private:

	FirstWindow* Window;

	GraphicsAPIConstructor* APIManager;

	InputManager* inputManager;

	World* world;

	Camera* camera;

	Renderer* renderer;

	LightManager* lightManager;

	CorePaths* PathManager;

};
