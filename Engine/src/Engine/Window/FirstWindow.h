// DO NOT MARK.
//This is because it has been submitted for my dissertation. Link to Original: https://github.falmouth.ac.uk/GA-Undergrad-Student-Work-25-26/Dissertation-SL295211.git


#pragma once
#include <cstdint>

#include "Window.h"
#include "glad/glad.h"
namespace OpenGL {

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
