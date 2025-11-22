#pragma once
#include <cstdint>

#include "Window.h"
#include "glad/glad.h"

namespace Vulkan
{

	class FirstWindow final : public Window
	{
	public:

		FirstWindow();

		~FirstWindow();

	private:

		void CreateWindow();

		void OnWindowResize() override;

		static bool bFrameBufferResized;

	};
}
