#pragma once
#include <cstdint>

#include "Window.h"
#include "glad/glad.h"
struct GLFWwindow;

namespace Vulkan
{

	class FirstWindow final : Window
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
