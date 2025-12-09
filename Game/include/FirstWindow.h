// DO NOT MARK.
//This is because it has been submitted for my dissertation. Link to Original: https://github.falmouth.ac.uk/GA-Undergrad-Student-Work-25-26/Dissertation-SL295211.git


#pragma once
#include <cstdint>

#include "Window.h"
#include "glad/glad.h"


class FirstWindow final : public Window 
{
public:

	FirstWindow();

	~FirstWindow();

	[[nodiscard]] inline GLFWwindow* GetWindow() const
	{
		return DisplayWindow;
	}

	static inline void SetWindowWidth(const GLint InWidth)
	{
		Width = InWidth;
	}

	static inline void SetWindowHeight(const GLint InHeight)
	{
		Height = InHeight;
	}

	[[nodiscard]] static inline GLint GetWindowWidth()
	{
		return Width;
	}
	[[nodiscard]] static inline GLint GetWindowHeight()
	{
		return Height;
	}

	[[nodiscard]] static inline bool HasWindowBeenResized()
	{
		return bFrameBufferResized;
	}


	static void ResetWindowResize()
	{
		bFrameBufferResized = false;
	}

private:

	void CreateWindow();


	void OnWindowResize() override;

	static bool bFrameBufferResized;

};
