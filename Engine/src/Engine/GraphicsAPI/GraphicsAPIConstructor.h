
#pragma once
#include "ErrorCodes.h"
#include "InitialiseVulkan.h"

enum GraphicsAPI : std::uint8_t
{
	OpenGl,
	Vulkan
};

class GraphicsAPIConstructor
{
public:

	ErrorCodes Init(const GraphicsAPI& Api);

	InitialiseVulkan vulkan;

	constexpr static float OpenGLVersion = 4.6f;

private:

};
