


#include "SubsystemInitialiser.h"

#include <exception>
#include <iostream>

#include "DataBuffers.h"
#include "DataBuffersOpenGL.h"
#include "HardwareDetails.h"
#include "WorldObject.h"
#include "ObjectFactory.h"
#include "Predictates.h"
#include "InitialiseOpenGL.h"
#include "OpenGLInstance.h"
#include "OpenGLShader.h"
#include "OpenGLTexture.h"

#include "HardwareDetails.h"
#include "ComputeShader/UComputeShader.h"
#include "ComputeShader/UOpenGLComputeShader.h"

namespace OpenGL {
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
			APIConstructer = new CInitialiseOpenGL();
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

		
		UHardwareDetails::GetGPU = [this] { return GetGPUInfo(); };

		UHardwareDetails::API = "OpenGL";

		::Shader::ShaderCreationFunc = OpenGL::Shader::CreateOpenGLShader;

		::Texture::TextureCreationFunc = OpenGL::Texture::CreateOpenGLTexture;

		::DataBuffers::APIBufferInstance = new OpenGL::DataBuffers();

		::UComputeShader::ShaderCreationFunc = OpenGL::UOpenGLComputeShader::CreateOpenGLComputeShader;

		try
		{
			renderer = new Renderer;
			SInstance::GetInstance()->Renderer = renderer;

		}
		catch (const std::exception& error)
		{
			std::cout << error.what() << "\n";
			return ERROR;
		}

		try
		{
			inputManager = new ::InputManager(Window->GetWindow());
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
			renderer->ShutDown();
			SInstance::GetInstance()->ShutDown();
			delete renderer;
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
