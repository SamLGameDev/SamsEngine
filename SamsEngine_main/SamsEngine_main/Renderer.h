#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "FirstWindow.h"
#include "LinkedList.h"
#include <unordered_map>
#include "Verticie.h"
#include "stb_image.h"

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

	std::unordered_map<int, Vertex> IndicesToVerts;

	std::unordered_map<Vertex, int> VertsToIndices;

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
		"layout (location = 1) in vec3 aColor;\n"
		"layout (location = 2) in vec2 aTexture;\n"

		"out vec3 VertexColor;\n"

		"out vec4 Pos; \n"

		"out vec2 TexCoord; \n"

		"uniform float offset; \n"

		"void main()\n"
		"{\n"
		"   Pos = vec4(aPos.x + -offset, aPos.y + offset, aPos.z, 1.0);\n"	
		"   gl_Position = Pos;\n" //can use negative to reverse
		"	VertexColor = aColor + offset;\n"
		"	TexCoord = aTexture;\n"
		"}\0";

	const char* Fragment = "#version 330 core\n"
		"out vec4 FragColor;\n"

		"in vec3 VertexColor;\n"

		"in vec4 Pos; \n"
		"in vec2 TexCoord; \n"

		"uniform sampler2D ourTexture; \n"
		"uniform sampler2D ourTexture2; \n"
		"uniform float Visibility; \n"


		"void main()\n"
		"{\n"
		"   FragColor = mix(texture(ourTexture, TexCoord), texture(ourTexture2, vec2(-TexCoord.x, TexCoord.y)), Visibility) * vec4(VertexColor, 1);\n"
		"}\0";
	const char* tempFragment = "#version 330 core\n"
		"out vec4 FragColor;\n"
		"void main()\n"
		"{\n"
		"   FragColor = vec4(0.5f, 0.5f, 0.2f, 1.0f);\n"
		"}\0";

};

