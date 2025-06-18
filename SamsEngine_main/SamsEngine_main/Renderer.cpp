
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


	unsigned int VAO[2];
	glGenVertexArrays(2, VAO);

	float* verts = new float[CurrentIndicesCount * 3];

	int* Inds = new int[CurrentIndicesCount];

	for (int i = 0; i < ItemsToRender.GetSize(); i++)
	{
		LinkedList<int> Indices = ItemsToRender[i]->GetIndices();
		for(int j = 0; j < Indices.GetSize(); j++)
		{
			Verticie vert = IndicesToVerts.find(Indices[j])->second;
			verts[Indices[j]] = vert.GetPosition().X;
			verts[Indices[j + 1]] = vert.GetPosition().Y;
			verts[Indices[j + 2]] = vert.GetPosition().Z;
			Inds[j + (i * 3)] = Indices[j];
		}
	}

	float AllVertsToRender;

	const int RenderSize = ItemsToRender.GetSize();

	GLuint* vbos = new GLuint[RenderSize];
	glGenBuffers(RenderSize, vbos);


	for (int i = 0; i < RenderSize; i++)
	{
		glBindVertexArray(VAO[i]);
		glBindBuffer(GL_ARRAY_BUFFER, vbos[i]);

		glBufferData(GL_ARRAY_BUFFER, ItemsToRender[i]->GetVerticiesSizeByte(), ItemsToRender[i]->GetVertices(), GL_STATIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);
	}

	while (!glfwWindowShouldClose(Window->GetWindow()))
	{
		WindowInputManager->ProcessInput(Window->GetWindow());
		glClearColor(0.5f, 0.2f, 0.7, 1);
		glClear(GL_COLOR_BUFFER_BIT);
		glUseProgram(tempShaderProgram);
		glBindVertexArray(VAO[1]);
		glDrawArrays(GL_TRIANGLES, 0, 3);
		glUseProgram(ShaderProgram);
		glBindVertexArray(VAO[0]);
		glDrawArrays(GL_TRIANGLES, 0, 3);



		glfwSwapBuffers(Window->GetWindow());
		glfwPollEvents();
	}
}
