#pragma once
#include <cmath>

enum EGraphicsAPI : std::uint8_t
{
	OpenGL,
	Vulkan
};

class Application
{
	Application(const EGraphicsAPI API = Vulkan);
};
