#include "VulkanSubsystemInitialiser.h"

#include <exception>
#include <iostream>

#include "DataBuffers.h"
#include "DataBuffersVulkan.h"
#include "HardwareDetails.h"
#include "WorldObject.h"
#include "ObjectFactory.h"
#include "VulkanInstance.h"
#include "VulkanWindow.h"
#include "Shader.h"
#include "VulkanShader.h"
#include "VulkanTexture.h"
#include "Texture.h"
#include "InterfaceRenderer.h"
#include "Predictates.h"
#include "UComputeShader.h"
#include "UVulkanComputeShader.h"

namespace Vulkan
{

	ErrorCodes SubsystemInitialiser::Init()
	{
		exactinit();
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

		::Texture::TextureCreationFunc = Vulkan::Texture::CreateVulkanTexture;

		::DataBuffers::APIBufferInstance = new Vulkan::DataBuffers();

		::UComputeShader::ShaderCreationFunc = Vulkan::UVulkanComputeShader::CreateVulkanComputeShader;

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

		UHardwareDetails::GetGPU = [this] {return GraphicsCard->GetGPUName(); };
		UHardwareDetails::API = "Vulkan";

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

			Camera::ProjectonDir = -1;

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
		delete camera;

		delete world;

		delete inputManager;

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
