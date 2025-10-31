#pragma once
#include <cstdint>

#include "Window.h"
#include "glad/glad.h"
struct GLFWwindow;


class FirstWindow final : public Window 
{
public:

	FirstWindow();

	~FirstWindow();

	[[nodiscard]] inline GLFWwindow* GetWindow() const
	{
		return Window;
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
	static GLFWwindow* Window;

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
