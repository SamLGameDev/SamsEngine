#pragma once
#include <cstdint>

#include "glad/glad.h"
struct GLFWwindow;


class FirstWindow final
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

	enum GraphicsAPI : std::uint8_t
	{
		OpenGl,
		Vulkan
	};

private:

	constexpr static std::uint8_t AntiAliasingSamples = 4;

	constexpr static float OpenGLVersion = 4.6f;

	constexpr static char WindowName[] = {"SamsEngine"};

	static GLint Width, Height;

	void Initialisation(const GraphicsAPI& API);

	void CreateWindow();

	static void FrameBuffer_Size_Callback(GLFWwindow* InWindow, const int InWidth, const int InHeight);

	GLFWwindow* Window;
};
