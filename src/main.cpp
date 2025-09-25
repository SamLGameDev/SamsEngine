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


	float lastTime = glfwGetTime();

	GLsync gsync = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0);

	while (!glfwWindowShouldClose(Camera::GetActiveWindow()->GetWindow()))
	{
		if (gsync)
		{
			while (true)
			{
				GLenum waitReturn = glClientWaitSync(gsync, GL_SYNC_FLUSH_COMMANDS_BIT, 1);
				if (waitReturn & (GL_ALREADY_SIGNALED | GL_CONDITION_SATISFIED))
				{
					break;
				}
			}
		}

		const float time = glfwGetTime();
		const float deltaTime = time - lastTime;
		lastTime = time;
		Object::TickDel.Broadcast(deltaTime);

		if (gsync)
		{
			glDeleteSync(gsync);
		}
		gsync = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0);
	}

	glfwTerminate();

	SubsystemManager.ShutDown();

	return EXIT_SUCCESS;
}
