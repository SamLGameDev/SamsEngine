#pragma once
#include <cstdint>
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

	static inline void SetWindowWidth(const float InWidth)
	{
		Width = InWidth;
	}

	static inline void SetWindowHeight(const float InHeight)
	{
		Height = InHeight;
	}

	[[nodiscard]] static inline float GetWindowWidth()
	{
		return Width;
	}
	[[nodiscard]] static inline float GetWindowHeight()
	{
		return Height;
	}

private:

	constexpr static std::uint8_t AntiAliasingSamples = 4;

	constexpr static float OpenGLVersion = 4.6;

	constexpr static char WindowName[] = {"SamsEngine"};

	static float Width, Height;

	void Initialisation();

	void CreateWindow();

	static void FrameBuffer_Size_Callback(GLFWwindow* InWindow, const int InWidth, const int InHeight);

	GLFWwindow* Window;
};
