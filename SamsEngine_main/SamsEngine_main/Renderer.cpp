
#include "Renderer.h"
#include "InputManager.h"
#include "Shape.h"
#include <iostream>
#include "Array.h"
#include "Vector2D.h"
#include "glm-1.0.1/glm/glm.hpp"
#include "glm-1.0.1/glm/gtc/matrix_transform.hpp"
#include "glm-1.0.1/glm/gtc/type_ptr.hpp"
#include "Shader.h"
#include "WireShapes.h"


LinkedList<WireObject*> Renderer::WiresToDraw;


Renderer::Renderer(InputManager* InInputManager)
{
	WindowInputManager = InInputManager;
}

Renderer::~Renderer()
{
}

void Renderer::AddItemToRender(Model* Item)
{
	ItemsToRender.Add(Item);
}

void Renderer::RenderingLoop()
{
	float lastFrameTime = glfwGetTime();

	DrawWireCube(Vector3D(0, 0, 0), Vector3D(5, 10, 10), Vector3D(0.2f, 0.5f, 0.2f));

	while (!glfwWindowShouldClose(Camera::GetActiveWindow()->GetWindow()))
	{
		float currentFrame = glfwGetTime();
		Camera::GetActiveCamera()->DeltaTime = currentFrame - lastFrameTime;
		lastFrameTime = currentFrame;

		//TickDel.Broadcast(deltaTime);

		//glClearColor(0.5f, 0.2f, 0.7, 1);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glEnable(GL_DEPTH_TEST);


		const float time = glfwGetTime();

		const float green = sin(time) / 2 + 0.5;

		Array<float> color;

		color.Add(0);
		color.Add(green);
		color.Add(0);
		color.Add(0);


	//	std::cout << WiresToDraw[0]->GetShader()->GetName() << std::endl;

		if (!ItemsToRender.IsEmpty())
		{
			for (unsigned int i = 0; i < ItemsToRender.GetSize(); i++)
			{
				ItemsToRender[i]->Draw();
			}
		}


	//	std::cout << WiresToDraw[0]->GetShader()->GetName() << std::endl;

		if (!WiresToDraw.IsEmpty())
		{
			for (unsigned int i = 0; i < WiresToDraw.GetSize(); i++)
			{
				WiresToDraw[i]->Draw();
			}
		}

		WindowInputManager->ProcessInput(Camera::GetActiveWindow()->GetWindow());

		glfwSwapBuffers(Camera::GetActiveWindow()->GetWindow());
		glfwPollEvents();
	}

	if (!WiresToDraw.IsEmpty())
	{
		for (unsigned int i = 0; i < WiresToDraw.GetSize(); i++)
		{
			delete WiresToDraw[i];
		}
	}

	//delete[] verts;
	//delete[] Inds;
}
