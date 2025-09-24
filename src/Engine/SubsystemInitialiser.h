
#pragma once

#include "ErrorCodes.h"
#include "FirstWindow.h"
#include "InputManager.h"

#include "World.h"
#include "Camera/Camera.h"
#include "Renderer.h"

#include <memory>

class SubsystemInitialiser
{
public:

	SubsystemInitialiser(){};

	ErrorCodes Init();

private:

	std::unique_ptr<FirstWindow> Window;

	std::unique_ptr<InputManager> inputManager;

	std::unique_ptr<World> world;

	std::unique_ptr<Camera> camera;

	std::unique_ptr<Renderer> renderer;
};
