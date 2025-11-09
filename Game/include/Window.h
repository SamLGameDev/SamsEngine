#pragma once
#include <cstdint>

#include "glad/glad.h"
struct GLFWwindow;

class Window
{
public:

	[[nodiscard]] virtual inline GLFWwindow* GetWindow() const
	{
		return DisplayWindow;
	}

	virtual inline void SetWindowWidth(const GLint InWidth)
	{
		Width = InWidth;
	}

	virtual inline void SetWindowHeight(const GLint InHeight)
	{
		Height = InHeight;
	}

	[[nodiscard]] virtual inline GLint GetWindowWidth()
	{
		return Width;
	}
	[[nodiscard]] virtual inline GLint GetWindowHeight()
	{
		return Height;
	}
	static GLFWwindow* DisplayWindow;

	[[nodiscard]] virtual inline bool HasWindowBeenResized()
	{
		return bFrameBufferResized;
	}

	virtual void ResetWindowResize()
	{
		bFrameBufferResized = false;
	}

	static Window* GetWindowInstance()
	{
		return Instance;
	}

protected:

	Window() = default;

	virtual ~Window() = default;

	constexpr static std::uint8_t AntiAliasingSamples = 4;

	constexpr static char WindowName[] = {"SamsEngine"};

	static GLint Width, Height;

	static void FrameBuffer_Size_Callback(GLFWwindow* InWindow, const int InWidth, const int InHeight);

	static bool bFrameBufferResized;

	virtual void OnWindowResize() = 0;

	static Window* Instance;

};
