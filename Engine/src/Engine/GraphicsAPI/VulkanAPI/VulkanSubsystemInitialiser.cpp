#include "VulkanSubsystemInitialiser.h"

#include <exception>
#include <iostream>

#include "DataBuffers.h"
#include "DataBuffersVulkan.h"
#include "WorldObject.h"
#include "ObjectFactory.h"
#include "VulkanInstance.h"
#include "VulkanWindow.h"
#include "Shader.h"
#include "VulkanShader.h"

namespace Vulkan
{

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
			APIConstructer = new InitialiseVulkan();
			APIConstructer->Init();
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

		::Shader::ShaderCreationFunc = Vulkan::Shader::CreateVulkanShader;

		::DataBuffers::APIBufferInstance = new Vulkan::DataBuffers();

		try
		{
			SInstance::GetInstance()->GraphicsCard = new UGraphicsCard();
			GraphicsCard = SInstance::GetInstance()->GraphicsCard;
			GraphicsCard->Init();
		}
		catch (const std::exception& error)
		{
			std::cerr << error.what() << "\n";
			return ERROR;
		}

	/*	try
		{
			inputManager = new InputManager();
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
		}*/

		return SUCCEEDED;
	}

	ErrorCodes SubsystemInitialiser::ShutDown()
	{

	///*	try
	//	{
	//		delete lightManager;

	//	}
	//	catch (const std::exception& error)
	//	{
	//		std::cout << error.what() << "\n";
	//		return ERROR;
	//	}

	//	try
	//	{
	//		delete renderer;
	//	}
	//	catch (const std::exception& error)
	//	{
	//		std::cout << error.what() << "\n";
	//		return ERROR;
	//	}

	//	try
	//	{
	//		delete camera;
	//	}
	//	catch (const std::exception& error)
	//	{
	//		std::cout << error.what() << "\n";
	//		return ERROR;
	//	}

	//	try
	//	{
	//		delete world;
	//	}
	//	catch (const std::exception& error)
	//	{
	//		std::cout << error.what() << "\n";
	//		return ERROR;
	//	}

	//	try
	//	{
	//		delete inputManager;
	//	}
	//	catch (const std::exception& error)
	//	{
	//		std::cout << error.what() << "\n";
	//		return ERROR;
	//	}*/

		 delete ::DataBuffers::APIBufferInstance;

		try
		{
			GraphicsCard->ShutDown();
			SInstance::GetInstance()->ShutDown();
		}
		catch (const std::exception& error)
		{
			std::cerr << error.what() << "\n";
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
			APIConstructer->Shutdown();
			delete APIConstructer;
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
}
