#pragma once

struct GLFWwindow;

class FirstWindow
{
public:

	FirstWindow();

	~FirstWindow();

	inline virtual GLFWwindow* GetWindow() const
	{
		return Window;
	}

	inline virtual void SetWindowWidth(const float InWidth)
	{
		Width = InWidth;
	}
	inline virtual void SetWindowHeight(const float InHeight)
	{
		Height = InHeight;
	}

	inline virtual float GetWindowWidth() const
	{
		return Width;
	}
	inline virtual float GetWindowHeight() const
	{
		return Height;
	}

private:

	static float Width, Height;

	void Initialisation();

	void CreateWindow();

	static void FrameBuffer_Size_Callback(GLFWwindow* InWindow, int InWidth, int InHeight);

	GLFWwindow* Window;

};

