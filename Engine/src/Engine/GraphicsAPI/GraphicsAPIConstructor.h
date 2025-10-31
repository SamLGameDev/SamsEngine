
#pragma once
#include "ErrorCodes.h"
#include "InitialiseVulkan.h"

enum GraphicsAPI : std::uint8_t
{
	OpenGl,
	DEPRECIATEDVulkan
};

class GraphicsAPIConstructor
{
public:


	ErrorCodes Init(const GraphicsAPI& Api);

	void AttachToWindow(const GraphicsAPI& Api);

	void Shutdown(const GraphicsAPI& API);

	InitialiseVulkan* vulkan;

	constexpr static float OpenGLVersion = 4.6f;

private:

};
