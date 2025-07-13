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
#include "Vector2D.h"
#include "Camera.h"



int main()
{

	FirstWindow* window = new FirstWindow();

	InputManager* inputManager = new InputManager(window->GetWindow());
	
	Camera* camera = new Camera(window, inputManager);


	Renderer* renderer = new Renderer(window, inputManager, camera);
	Vertex vert = Vertex(Vector3D(0.5f, 0.5f, 0));
	Vertex vert2 = Vertex(Vector3D(0.5f, -0.5f, 0));
	Vertex vert3 = Vertex(Vector3D(-0.5f, 0.5f, 0));

	renderer->AddItemToRender(new Triangle(vert, vert2, vert3, Vector3D(0.5f, 0.2f, 0.5f), Vector2D(2,2), Vector2D(2, 0), Vector2D(0, 2)));
	Vertex vertx = Vertex(Vector3D(0.5f, -0.5f, 0));
	Vertex vertx2 = Vertex(Vector3D(-0.5f, -0.5f, 0));
	Vertex vertx3 = Vertex(Vector3D(-0.5f, 0.5f, 0));

	renderer->AddItemToRender(new Triangle(vertx, vertx2, vertx3, Vector3D(0.5f, 0, 0), Vector2D(2, 0), Vector2D(0, 0), Vector2D(0, 2)));

	Vertex verty = Vertex(Vector3D(0.5f, -0.5f, 0));
	Vertex verty2 = Vertex(Vector3D(-0.5f, -0.5f, 0));
	Vertex verty3 = Vertex(Vector3D(0, 0.9f, 0));

	//renderer->AddItemToRender(new Triangle(verty, verty2, verty3, Vector3D(0.5f, 0.5f, 0.9f)));

	renderer->Start();

	glfwTerminate();
	return 0;
}
