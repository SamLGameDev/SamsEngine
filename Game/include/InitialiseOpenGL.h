
#pragma once

#include "ErrorCodes.h"

namespace OpenGL
{
	class Renderer;

	class CInitialiseOpenGL
	{
	public:

		CInitialiseOpenGL() = default;

		ErrorCodes Init();

		ErrorCodes Shutdown();

	private:

		constexpr static float OpenGLVersion = 4.6f;

	};
}
