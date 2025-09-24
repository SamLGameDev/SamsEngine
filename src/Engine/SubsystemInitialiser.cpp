#include "SubsystemInitialiser.h"

#include <exception>
#include <iostream>
#include "WorldObject.h"

ErrorCodes SubsystemInitialiser::Init()
{
	try
	{
		Window = FirstWindow();
	}
	catch (const std::exception& error)
	{
		std::cout << error.what() << "\n";
		return ERROR;
	}

	try
	{
		inputManager = InputManager(Window.GetWindow());
	}
	catch (const std::exception& error)
	{
		std::cout << error.what() << "\n";
		return ERROR;
	}

	try
	{
		world = CreateObjectRaw<World>();
		WorldObject::World = &world;

	}
	catch (const std::exception& error)
	{
		std::cout << error.what() << "\n";
		return ERROR;
	}



	return SUCCEEDED;
}
