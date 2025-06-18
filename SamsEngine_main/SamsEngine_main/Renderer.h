#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "FirstWindow.h"
#include "LinkedList.h"
#include <unordered_map>
#include "Verticie.h"

class InputManager;
class Shape;

class Renderer
{

public:
	Renderer(FirstWindow* InWindow, InputManager* InInputManager);
	~Renderer();

	void AddItemToRender(Shape* Item);

	bool Start()
	{
		RenderingLoop();
		return true;
	}

	LinkedList<Shape*> ItemsToRender;

private:

	std::unordered_map<int, Verticie> IndicesToVerts;

	std::unordered_map<Verticie, int> VertsToIndices;

	LinkedList<int> FreeIndices;

	int CurrentIndicesCount = 0;

	void RenderingLoop();

	FirstWindow* Window;

	InputManager* WindowInputManager;
	unsigned int VertexShader;

	unsigned int FragmentShader;

	unsigned int ShaderProgram;

	const char* Shader = "#version 330 core\n"
		"layout (location = 0) in vec3 aPos;\n"
		"void main()\n"
		"{\n"
		"   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
		"}\0";

	const char* Fragment = "#version 330 core\n"
		"out vec4 FragColor;\n"
		"void main()\n"
		"{\n"
		"   FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
		"}\0";
	const char* tempFragment = "#version 330 core\n"
		"out vec4 FragColor;\n"
		"void main()\n"
		"{\n"
		"   FragColor = vec4(0.5f, 0.5f, 0.2f, 1.0f);\n"
		"}\0";

};

