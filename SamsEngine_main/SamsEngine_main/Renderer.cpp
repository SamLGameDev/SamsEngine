
#include "Renderer.h"
#include "InputManager.h"
#include "Shape.h"
#include <iostream>
#include "Array.h"




Renderer::Renderer(FirstWindow* InWindow, InputManager* InInputManager)
{
	Window = InWindow;
	WindowInputManager = InInputManager;
}

Renderer::~Renderer()
{
}

void Renderer::AddItemToRender(Shape* Item)
{
	LinkedList<int> Indices;
	for (int i =0; i < 3; i++)
	{
		Verticie Vert = Item->GetVerts()[i];
		auto KeyValue = VertsToIndices.find(Vert);
		if (KeyValue == VertsToIndices.end())
		{
			if (FreeIndices.IsEmpty())
			{
				VertsToIndices.insert(std::pair<const Verticie, int>(Vert, CurrentIndicesCount));
				IndicesToVerts.insert(std::pair<const int, Verticie>(CurrentIndicesCount, Vert));
				Indices.Add(CurrentIndicesCount);
				CurrentIndicesCount++;
			}
			else
			{
				VertsToIndices.insert(std::pair<const Verticie, int>(Vert, FreeIndices[0]));
				IndicesToVerts.insert(std::pair<const int, Verticie>(FreeIndices[0], Vert));
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


	int totsSzeverts = VertsToIndices.size() * 3;

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
			Verticie vert = IndicesToVerts.find(index)->second;

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


	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);


	while (!glfwWindowShouldClose(Window->GetWindow()))
	{
		WindowInputManager->ProcessInput(Window->GetWindow());
		glClearColor(0.5f, 0.2f, 0.7, 1);
		glClear(GL_COLOR_BUFFER_BIT);

		glUseProgram(ShaderProgram);
		glDrawElements(GL_TRIANGLES, ItemsToRender.GetSize() * 3, GL_UNSIGNED_INT, 0);



		glfwSwapBuffers(Window->GetWindow());
		glfwPollEvents();
	}

	//delete[] verts;
	//delete[] Inds;
}
