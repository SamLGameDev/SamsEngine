
#pragma once

#include "ErrorCodes.h"
#include "FirstWindow.h"
#include "InputManager.h"

#include "World.h"
#include "Camera.h"
#include "Renderer.h"
#include "LightManager.h"
#include "CorePaths.h"
// DO NOT MARK.
//This is because it has been submitted for my dissertation. Link to Original: https://github.falmouth.ac.uk/GA-Undergrad-Student-Work-25-26/Dissertation-SL295211.git


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

		CInitialiseOpenGL* APIConstructer;

		FirstWindow* Window;

		::InputManager* inputManager;

		World* world;

		Camera* camera;

		OpenGL::Renderer* renderer;

		LightManager* lightManager;

		CorePaths* PathManager;

	};
}
