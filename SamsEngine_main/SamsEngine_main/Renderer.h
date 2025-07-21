#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "FirstWindow.h"
#include "LinkedList.h"
#include <unordered_map>
#include "Verticie.h"
#include "stb_image.h"
#include "Camera.h"
#include "Renderer.h"

class InputManager;
class Shape;

class Renderer
{

public:
	Renderer(FirstWindow* InWindow, InputManager* InInputManager, Camera* InCamera);
	~Renderer();

	void AddItemToRender(Shape* Item);

	bool Start()
	{
		RenderingLoop();
		return true;
	}

	LinkedList<Shape*> ItemsToRender;

	//static MulticastDelegate<float> TickDel;
private:

	Camera* Cam;

	std::unordered_map<int, Vertex> IndicesToVerts;

	std::unordered_map<Vertex, int> VertsToIndices;

	LinkedList<int> FreeIndices;

	int CurrentIndicesCount = 0;

	void RenderingLoop();

	FirstWindow* Window;

	InputManager* WindowInputManager;


};

