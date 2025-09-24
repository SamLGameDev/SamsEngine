#include "SubsystemInitialiser.h"

#include <exception>
#include <iostream>
#include "WorldObject.h"
#include "ObjectFactory.h"

ErrorCodes SubsystemInitialiser::Init()
{
	try
	{
		Window = std::make_unique<FirstWindow>();
	}
	catch (const std::exception& error)
	{
		std::cout << error.what() << "\n";
		return ERROR;
	}

	try
	{
		inputManager = std::make_unique<InputManager>(Window->GetWindow());
	}
	catch (const std::exception& error)
	{
		std::cout << error.what() << "\n";
		return ERROR;
	}

	try
	{
		world = std::make_unique<World>(CreateObjectRaw<World>());
		WorldObject::World = world.get();

	}
	catch (const std::exception& error)
	{
		std::cout << error.what() << "\n";
		return ERROR;
	}

	try
	{
		camera = std::unique_ptr<Camera>(CreateObjectPtr<Camera>(Window.get(), inputManager.get()));
		Camera::SetActiveCamera(camera.get());

		Camera::SetActiveWindow(Window.get());

	}
	catch (const std::exception& error)
	{
		std::cout << error.what() << "\n";
		return ERROR;
	}

	try
	{
		renderer = std::unique_ptr<Renderer>(CreateObjectPtr<Renderer>(inputManager.get()));

	}
	catch (const std::exception& error)
	{
		std::cout << error.what() << "\n";
		return ERROR;
	}

	return SUCCEEDED;
}
