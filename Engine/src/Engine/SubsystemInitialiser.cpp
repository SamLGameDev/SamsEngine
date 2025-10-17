#include "SubsystemInitialiser.h"

#include <exception>
#include <iostream>
#include "WorldObject.h"
#include "ObjectFactory.h"


ErrorCodes SubsystemInitialiser::Init()
{

	try
	{
		PathManager = new CorePaths;
	}
	catch (const std::exception& error)
	{
		std::cout << error.what() << "\n";
		return ERROR;
	}


	try
	{
		Window = new FirstWindow();
	}
	catch (const std::exception& error)
	{
		std::cout << error.what() << "\n";
		return ERROR;
	}

	try
	{
		APIManager = new GraphicsAPIConstructor();
		APIManager->Init(Vulkan);
	}
	catch (const std::exception& error)
	{
		std::cout << error.what() << "\n";
		return ERROR;
	}

	try
	{
		inputManager = new InputManager(Window->GetWindow());
	}
	catch (const std::exception& error)
	{
		std::cout << error.what() << "\n";
		return ERROR;
	}

	try
	{
		world = CreateObjectPtr<World>();
		WorldObject::World = world;

	}
	catch (const std::exception& error)
	{
		std::cout << error.what() << "\n";
		return ERROR;
	}

	try
	{
		camera = CreateObjectPtr<Camera>(Window, inputManager);
		Camera::SetActiveCamera(camera);

		Camera::SetActiveWindow(Window);

	}
	catch (const std::exception& error)
	{
		std::cout << error.what() << "\n";
		return ERROR;
	}

	try
	{
		renderer = CreateObjectPtr<Renderer>(inputManager);

	}
	catch (const std::exception& error)
	{
		std::cout << error.what() << "\n";
		return ERROR;
	}

	try
	{
		lightManager = new LightManager();

	}
	catch (const std::exception& error)
	{
		std::cout << error.what() << "\n";
		return ERROR;
	}

	return SUCCEEDED;
}

ErrorCodes SubsystemInitialiser::ShutDown()
{

	try
	{
		delete lightManager;

	}
	catch (const std::exception& error)
	{
		std::cout << error.what() << "\n";
		return ERROR;
	}

	try
	{
		delete renderer;
	}
	catch (const std::exception& error)
	{
		std::cout << error.what() << "\n";
		return ERROR;
	}

	try
	{
		delete camera;
	}
	catch (const std::exception& error)
	{
		std::cout << error.what() << "\n";
		return ERROR;
	}

	try
	{
		delete world;
	}
	catch (const std::exception& error)
	{
		std::cout << error.what() << "\n";
		return ERROR;
	}

	try
	{
		delete inputManager;
	}
	catch (const std::exception& error)
	{
		std::cout << error.what() << "\n";
		return ERROR;
	}

	try
	{
		delete APIManager;
	}
	catch (const std::exception& error)
	{
		std::cout << error.what() << "\n";
		return ERROR;
	}


	try
	{
		delete Window;
	}
	catch (const std::exception& error)
	{
		std::cout << error.what() << "\n";
		return ERROR;
	}

	try
	{
		delete PathManager;
	}
	catch (const std::exception& error)
	{
		std::cout << error.what() << "\n";
		return ERROR;
	}

	return SUCCEEDED;
}
