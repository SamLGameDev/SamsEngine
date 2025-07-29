
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


Renderer::Renderer(FirstWindow* InWindow, InputManager* InInputManager, Camera* InCamera)
{
	Window = InWindow;
	WindowInputManager = InInputManager;
	Cam = InCamera;
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
	glm::mat4 translation = glm::mat4(1);
	translation = glm::translate(translation, glm::vec3(0, 0, 0.0f));
	translation = glm::rotate(translation, glm::radians(0.0f), glm::vec3(0, 0, 1));
	translation = glm::scale(translation, glm::vec3(0.5, 0.5, 0.5));

	glm::mat4 Model = glm::mat4(1);
	Model = glm::rotate(Model, glm::radians(-55.0f), glm::vec3(1, 0, 0));

	glm::mat4 View = glm::mat4(1);

	glm::mat4 projection = glm::perspective(glm::radians(45.0f), Window->GetWindowWidth() / Window->GetWindowHeight(), 0.1f, 100.0f);

	float lastFrameTime = glfwGetTime();

	glm::mat3 NormalModel = glm::transpose(glm::inverse(View * Model));
	while (!glfwWindowShouldClose(Window->GetWindow()))
	{
		float currentFrame = glfwGetTime();
		Cam->DeltaTime = currentFrame - lastFrameTime;
		lastFrameTime = currentFrame;

		//TickDel.Broadcast(deltaTime);

		glClearColor(0.5f, 0.2f, 0.7, 1);
		glClear(GL_COLOR_BUFFER_BIT);

		const float time = glfwGetTime();

		const float green = sin(time) / 2 + 0.5;

		Array<float> color;

		color.Add(0);
		color.Add(green);
		color.Add(0);
		color.Add(0);

		View = Cam->GetLook();

		Shader* MeshShader = ItemsToRender[0]->GetShader();

		MeshShader->Use();

		MeshShader->SetMatrix4fv("Transform", glm::value_ptr(translation));

		MeshShader->SetMatrix4fv("Model", glm::value_ptr(Model));

		MeshShader->SetMatrix4fv("View", glm::value_ptr(View));

		MeshShader->SetMatrix4fv("Projection", glm::value_ptr(projection));

		MeshShader->SetMatrix3fv("NormalModel", glm::value_ptr(NormalModel));

		glm::vec3 ViewPos = View * Model * glm::vec4(0, 0.5f, 0, 1);

		MeshShader->SetVec3("light.position", Vector3D(ViewPos.x, ViewPos.y, ViewPos.z));


		MeshShader->SetVec3("material.ambient", Vector3D(1.0f, 0.5f, 0.31f));
		MeshShader->SetFloat("material.shininess", 32.0f);

		MeshShader->SetVec3("material.diffuse", Vector3D(1.0f, 0.5f, 0.31f));
		MeshShader->SetVec3("material.specular", Vector3D(0.5f, 0.5f, 0.5f));

		MeshShader->SetVec3("light.ambient", Vector3D(0.2f, 0.2f, 0.2f));
		MeshShader->SetVec3("light.diffuse", Vector3D(0.5f, 0.5f, 0.5f)); // darken diffuse light a bit
		MeshShader->SetVec3("light.specular",Vector3D(1.0f, 1.0f, 1.0f));

		ItemsToRender[0]->Draw();

		WindowInputManager->ProcessInput(Window->GetWindow());

		glfwSwapBuffers(Window->GetWindow());
		glfwPollEvents();
	}

	//delete[] verts;
	//delete[] Inds;
}
