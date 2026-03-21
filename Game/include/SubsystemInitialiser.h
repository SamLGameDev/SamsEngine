
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

namespace OpenGL {
	class CInitialiseOpenGL;

	class SubsystemInitialiser
	{
	public:

		SubsystemInitialiser() {};

		ErrorCodes Init();

		ErrorCodes ShutDown();

	private:

		std::string GetGPUInfo();


		CInitialiseOpenGL* APIConstructer;

		FirstWindow* Window;

		::InputManager* inputManager;

		World* world;

		Camera* camera;

		OpenGL::Renderer* renderer;

		LightManager* lightManager;

		CorePaths* PathManager;

	};

	inline std::string SubsystemInitialiser::GetGPUInfo()
	{
		const std::string gpu = reinterpret_cast<const char*>(glGetString(GL_RENDERER));
		return gpu;
	}
}
