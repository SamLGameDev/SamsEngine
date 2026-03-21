
#pragma once

#include <optional>

#include "ErrorCodes.h"
#include "FirstWindow.h"

namespace OpenGL
{
	class Renderer;

	class SInstance
	{
	public:


		static SInstance* GetInstance();

		ErrorCodes ShutDown();

		Renderer* Renderer;

	private:

		SInstance() = default;

		static std::optional<SInstance*> Instance;

	};
}
