#include <project/main.hpp>

#include <iostream>
#include <cstdlib>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "SubsystemInitialiser.h"
#include "Camera.h"

int main(int argc, char* argv[]) {

	
	SubsystemInitialiser SubsystemManager;

	ErrorCodes status = SubsystemManager.Init();

	if (status == ERROR)
	{
		return EXIT_FAILURE;
	}


	while(true)
	{
		glfwPollEvents();
		glfwSwapBuffers(Camera::GetActiveWindow()->GetWindow());
	}

	std::cout << "Hello, world!" << std::endl;
	return EXIT_SUCCESS;
}
