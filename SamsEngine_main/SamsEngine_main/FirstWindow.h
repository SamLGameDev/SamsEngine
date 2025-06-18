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

private:

	void Initialisation();

	void CreateWindow();

	static void FrameBuffer_Size_Callback(GLFWwindow* InWindow, int Width, int Height);

	GLFWwindow* Window;

};

