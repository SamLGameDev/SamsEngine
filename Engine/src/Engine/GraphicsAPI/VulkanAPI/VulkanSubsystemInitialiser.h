
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

		Renderer* renderer;

		LightManager* lightManager;

		CorePaths* PathManager;

		UGraphicsCard* GraphicsCard;

	};
}
