// DO NOT MARK.
//This is because it has been submitted for my dissertation. Link to Original: https://github.falmouth.ac.uk/GA-Undergrad-Student-Work-25-26/Dissertation-SL295211.git


#include "RuntimeEngine.h"
#include "Camera.h"
#include "OpenGLInstance.h"

namespace OpenGL {

	ErrorCodes RuntimeEngine::Init()
	{
		SubsystemManager = new SubsystemInitialiser;

		ErrorCodes status = SubsystemManager->Init();

		if (status == ERROR)
		{
			return ERROR;
		}

		TimeLastFrame = glfwGetTime();

		Gsync = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0);

		return SUCCEEDED;
	}

	ErrorCodes RuntimeEngine::Loop()
	{
		if (Gsync)
		{
			while (true)
			{
				GLenum waitReturn = glClientWaitSync(Gsync, GL_SYNC_FLUSH_COMMANDS_BIT, 1);
				if (waitReturn & (GL_ALREADY_SIGNALED | GL_CONDITION_SATISFIED))
				{
					break;
				}
			}
		}

		const double time = glfwGetTime();
		const double deltaTime = time - TimeLastFrame;
		TimeLastFrame = time;
		Object::TickDel.Broadcast(deltaTime);

		SInstance::GetInstance()->Renderer->RenderingLoop();

		if (Gsync)
		{
			glDeleteSync(Gsync);
		}
		Gsync = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0);

		return SUCCEEDED;
	}

	ErrorCodes RuntimeEngine::ShutDown()
	{

		SubsystemManager->ShutDown();

		delete SubsystemManager;

		Object::TickDel.RemoveAll();

		Model::LoadedTextures.Empty();

		glfwTerminate();

		return SUCCEEDED;
	}

	bool RuntimeEngine::ShouldClose()
	{
		return glfwWindowShouldClose(Camera::GetActiveWindow()->GetWindow());
	}
}
