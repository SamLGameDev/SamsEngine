#pragma once

#include "glm-1.0.1/glm/glm.hpp"
#include "FirstWindow.h"
#include "InputManager.h"
#include "InputAction.h"
#include "InputActionMouse.h"

class Camera
{
public:

	Camera(FirstWindow* Window, InputManager* Manager);

	void MoveForward();

	void MoveBackwards();

	void MoveLeft();

	void MoveRight();

	void MouseCallback(float xpos, float ypos);

	glm::vec3 GetPos() const
	{
		return Pos;
	}

	const glm::mat4 GetLook() const;

	float DeltaTime;

	static Camera* GetActiveCamera()
	{
		return ActiveCamera;
	};

	static void SetActiveCamera(Camera* NewCamera)
	{
		ActiveCamera = NewCamera;
	}

	static FirstWindow* GetActiveWindow()
	{
		return ActiveWindow;
	};

	static void SetActiveWindow(FirstWindow* NewWindow)
	{
		ActiveWindow = NewWindow;
	}

	const glm::mat4 GetProjection()const;

private:

	float Yaw = -90;
	float Pitch = 0;

	bool bIsFirstMouse = true;

	float LastX, LastY;

	const float Speed = 3.0f;

	const float LookSpeed = 0.1f;

	const float FOV = 45.0f;

	InputAction* WKey;
	InputAction* AKey;
	InputAction* SKey;
	InputAction* DKey;

	InputActionMouse Mouse;


	glm::vec3 Pos = glm::vec3(0, 0, 3);
	glm::vec3 Front = glm::vec3(0, 0, -1);
	glm::vec3 Up = glm::vec3(0, 1, 0);

	glm::vec3 Directon = glm::vec3(0, 0, 0);

	static Camera* ActiveCamera;

	static FirstWindow* ActiveWindow;



};

