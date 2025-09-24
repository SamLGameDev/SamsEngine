#include <project/main.hpp>

#include <iostream>
#include <cstdlib>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "Array.h"
#include "Shader.h"

int main(int argc, char* argv[]) {

	Array<int> h = {5, 3 ,1};

	std::cout << h[1];

	

	float OpenGLVersion = 3.3;

	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, (int)OpenGLVersion);

	//do this to get the first digit
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, (int)(OpenGLVersion * 10) % 10);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_SAMPLES, 4);
	GLFWwindow* Window = glfwCreateWindow(800, 600, "SamsEngine", NULL, NULL);

	if (Window == NULL)
	{
#if DEBUG
		std::cout << "Failed to create window" << std::endl;
#endif
		glfwTerminate();
	}

	glfwMakeContextCurrent(Window);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
#if DEBUG
		std::cout << "Failed to initialise glad" << std::endl;
#endif
	}


	while(true)
	{
		glfwPollEvents();
		glfwSwapBuffers(Window);
	}

	std::cout << "Hello, world!" << std::endl;
	return EXIT_SUCCESS;
}
