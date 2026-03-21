// DO NOT MARK 
//This is because it has been submitted for my COMP305. Link to Original: https://github.falmouth.ac.uk/GA-Undergrad-Student-Work-25-26/Comp305-Engine-SL295211.git
#pragma once

#include "ErrorCodes.h"
#include "VulkanWindow.h"
#include "InputManager.h"

#include "World.h"
#include "Camera.h"
#include "Renderer.h"
#include "LightManager.h"
#include "CorePaths.h"
#include "GraphicsAPIConstructor.h"

#include <memory>

#include "VulkanGraphicsCard.h"
#include "VulkanRenderPass.h"

namespace Vulkan
{

	class SubsystemInitialiser
	{
	public:

		SubsystemInitialiser() {};

		ErrorCodes Init();

		ErrorCodes ShutDown();

	private:

		FirstWindow* Window;

		InitialiseVulkan* APIConstructer;

		InputManager* inputManager;

		World* world;

		Camera* camera;

		CorePaths* PathManager;

		UGraphicsCard* GraphicsCard;

	};
}
