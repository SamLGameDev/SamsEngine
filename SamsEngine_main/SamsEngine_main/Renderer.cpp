
#include "Renderer.h"
#include "InputManager.h"
#include "Shape.h"
#include <iostream>
#include "Array.h"
#include "Vector2D.h"
#include "glm-1.0.1/glm/glm.hpp"
#include "glm-1.0.1/glm/gtc/matrix_transform.hpp"
#include "glm-1.0.1/glm/gtc/type_ptr.hpp"

Renderer::Renderer(FirstWindow* InWindow, InputManager* InInputManager, Camera* InCamera)
{
	Window = InWindow;
	WindowInputManager = InInputManager;
	Cam = InCamera;
}

Renderer::~Renderer()
{
}

void Renderer::AddItemToRender(Shape* Item)
{
	LinkedList<int> Indices;
	for (int i =0; i < 3; i++)
	{
		Vertex Vert = Item->GetVerts()[i];
		auto KeyValue = VertsToIndices.find(Vert);
		if (KeyValue == VertsToIndices.end())
		{
			if (FreeIndices.IsEmpty())
			{
				VertsToIndices.insert(std::pair<const Vertex, int>(Vert, CurrentIndicesCount));
				IndicesToVerts.insert(std::pair<const int, Vertex>(CurrentIndicesCount, Vert));
				Indices.Add(CurrentIndicesCount);
				CurrentIndicesCount++;
			}
			else
			{
				VertsToIndices.insert(std::pair<const Vertex, int>(Vert, FreeIndices[0]));
				IndicesToVerts.insert(std::pair<const int, Vertex>(FreeIndices[0], Vert));
				Indices.Add(FreeIndices[0]);
				FreeIndices.RemoveAt(0);
			}

		}
		else
		{
			Indices.Add(KeyValue->second);
		}
	}
	Item->SetRenderOrder(Indices);
	ItemsToRender.Add(Item);

	return;
}

void Renderer::RenderingLoop()
{
	VertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(VertexShader, 1, &Shader, NULL);
	glCompileShader(VertexShader);
	int  success;
	char infoLog[512];
	glGetShaderiv(VertexShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(VertexShader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
	}
	FragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(FragmentShader, 1, &Fragment, NULL);
	glCompileShader(FragmentShader);
	glGetShaderiv(FragmentShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(FragmentShader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
	}
	ShaderProgram = glCreateProgram();
	glAttachShader(ShaderProgram, VertexShader);
	glAttachShader(ShaderProgram, FragmentShader);
	glLinkProgram(ShaderProgram);
	glGetProgramiv(ShaderProgram, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(ShaderProgram, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::PROGRAM::COMPILATION_FAILED\n" << infoLog << std::endl;
	}
	unsigned int tempFragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(tempFragmentShader, 1, &tempFragment, NULL);
	glCompileShader(tempFragmentShader);
	glGetShaderiv(tempFragmentShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(tempFragmentShader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
	}
	unsigned int tempShaderProgram = glCreateProgram();
	glAttachShader(tempShaderProgram, VertexShader);
	glAttachShader(tempShaderProgram, tempFragmentShader);
	glLinkProgram(tempShaderProgram);
	glDeleteShader(VertexShader);
	glDeleteShader(FragmentShader);
	glDeleteShader(tempFragmentShader);


	unsigned int VBO, VAO, EBO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	int totsSzeverts = VertsToIndices.size() * 8;

	float debugverts[100];

	int debugInds[100];

	float* verts = new float[totsSzeverts]();

	int totsSze = 0;

	for (int i = 0; i < ItemsToRender.GetSize(); i++)
	{
		LinkedList<int> Indices = ItemsToRender[i]->GetIndices();
		totsSze += Indices.GetSize();
	}

	int* Inds = new int[totsSze]();

	int next = 0;

	LinkedList<int> AppearedIndicies;

	for (int i = 0; i < ItemsToRender.GetSize(); i++)
	{
		LinkedList<int> Indices = ItemsToRender[i]->GetIndices();

		for(int j = 0; j < Indices.GetSize(); j++)
		{
			int index = Indices[j];
			Vertex vert = IndicesToVerts.find(index)->second;

			if (!AppearedIndicies.Contains(index))
			{
				verts[next] = vert.GetPosition().X;
				debugverts[next] = vert.GetPosition().X;
				next++;
				verts[next] = vert.GetPosition().Y;
				debugverts[next] = vert.GetPosition().Y;
				next++;
				verts[next] = vert.GetPosition().Z;
				debugverts[next] = vert.GetPosition().Z;
				next++;
				verts[next] = ItemsToRender[i]->GetColor().X;
				next++;
				verts[next] = ItemsToRender[i]->GetColor().Y;
				next++;
				verts[next] = ItemsToRender[i]->GetColor().Z;
				next++;
				verts[next] = ItemsToRender[i]->GetTextureCords()[j].X;
				next++;
				verts[next] = ItemsToRender[i]->GetTextureCords()[j].Y;
				next++;

				AppearedIndicies.Add(index);
			}



			debugInds[j + (i * 3)]= Indices[j];
			Inds[j + (i * 3)] = Indices[j];
		}
	}

	float AllVertsToRender;

	const int RenderSize = ItemsToRender.GetSize();

	std::cout << "VERTS:\n";
	for (int i = 0; i < totsSzeverts; i += 3) {
		std::cout << verts[i] << ", " << verts[i + 1] << ", " << verts[i + 2] << "\n";
	}

	std::cout << "INDICES:\n";
	for (int i = 0; i < totsSze; i += 3) {
		std::cout << Inds[i] << ", " << Inds[i + 1] << ", " << Inds[i + 2] << "\n";
	}

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, totsSzeverts * sizeof(float), verts, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, totsSze* sizeof(int), Inds, GL_STATIC_DRAW);


	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);

	glm::mat4 translation = glm::mat4(1);
	translation = glm::translate(translation, glm::vec3(0, 0, 0.0f));
	translation = glm::rotate(translation, glm::radians(0.0f), glm::vec3(0, 0, 1));
	translation = glm::scale(translation, glm::vec3(0.5, 0.5, 0.5));

	glm::mat4 Model = glm::mat4(1);
	Model = glm::rotate(Model, glm::radians(-55.0f), glm::vec3(1, 0, 0));

	glm::mat4 View = glm::mat4(1);

	glm::mat4 projection = glm::perspective(glm::radians(45.0f), Window->GetWindowWidth() / Window->GetWindowHeight(), 0.1f, 100.0f);

	float lastFrameTime = glfwGetTime();
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

		int UniformColorLoc = glGetUniformLocation(ShaderProgram, "VertexColor");

		int UniformOffSetLoc = glGetUniformLocation(ShaderProgram, "offset");

		unsigned int UniformTransform = glGetUniformLocation(ShaderProgram, "Transform");


		unsigned int UniformModel = glGetUniformLocation(ShaderProgram, "Model");


		unsigned int UniformView = glGetUniformLocation(ShaderProgram, "View");


		unsigned int UniformProjection = glGetUniformLocation(ShaderProgram, "Projection");

		glUseProgram(ShaderProgram);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, ItemsToRender[0]->GetTexture(0));

		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, ItemsToRender[1]->GetTexture(1));

		View = Cam->GetLook();

		glUniform1i(glGetUniformLocation(ShaderProgram, "ourTexture"), 0);
		glUniform1i(glGetUniformLocation(ShaderProgram, "ourTexture2"), 1);
		glUniformMatrix4fv(UniformTransform, 1, GL_FALSE, glm::value_ptr(translation));
		glUniformMatrix4fv(UniformModel, 1, GL_FALSE, glm::value_ptr(Model));
		glUniformMatrix4fv(UniformView, 1, GL_FALSE, glm::value_ptr(View));
		glUniformMatrix4fv(UniformProjection, 1, GL_FALSE, glm::value_ptr(projection));

		glUseProgram(ShaderProgram);

		glUniform4f(UniformColorLoc, 0, green, 0, 1);

		glUniform1f(UniformOffSetLoc, green);

		glDrawElements(GL_TRIANGLES, ItemsToRender.GetSize() * 3, GL_UNSIGNED_INT, 0);


		WindowInputManager->ProcessInput(Window->GetWindow(), ShaderProgram);

		glfwSwapBuffers(Window->GetWindow());
		glfwPollEvents();
	}

	//delete[] verts;
	//delete[] Inds;
}
