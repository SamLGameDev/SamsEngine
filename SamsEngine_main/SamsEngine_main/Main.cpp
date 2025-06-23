#include <glad/glad.h>
#include "Main.h"
#include <GLFW/glfw3.h>
#include <iostream>
#include "FirstWindow.h"
#include "Renderer.h"
#include "InputManager.h"
#include "Triangle.h"
#include "Verticie.h"
#include "Vector3D.h"
#include "LinkedList.h"



int main()
{

	FirstWindow* window = new FirstWindow();

	InputManager* inputManager = new InputManager();
	Renderer* renderer = new Renderer(window, inputManager);
	Verticie vert = Verticie(Vector3D(0.5f, 0.5f, 0));
	Verticie vert2 = Verticie(Vector3D(0.5f, -0.5f, 0));
	Verticie vert3 = Verticie(Vector3D(-0.5f, 0.5f, 0));

	renderer->AddItemToRender(new Triangle(vert, vert2, vert3));
	Verticie vertx = Verticie(Vector3D(0.5f, -0.5f, 0));
	Verticie vertx2 = Verticie(Vector3D(-0.5f, -0.5f, 0));
	Verticie vertx3 = Verticie(Vector3D(-0.5f, 0.5f, 0));

	renderer->AddItemToRender(new Triangle(vertx, vertx2, vertx3));

	Verticie verty = Verticie(Vector3D(0.5f, -0.5f, 0));
	Verticie verty2 = Verticie(Vector3D(-0.5f, -0.5f, 0));
	Verticie verty3 = Verticie(Vector3D(0, 0.9f, 0));

	renderer->AddItemToRender(new Triangle(verty, verty2, verty3));

	renderer->Start();

	glfwTerminate();
	return 0;
}
