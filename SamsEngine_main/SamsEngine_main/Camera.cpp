#include "Camera.h"
#include "glm-1.0.1/glm/gtc/matrix_transform.hpp"



Camera::Camera(FirstWindow* Window, InputManager* Manager)
{
	WKey = new InputAction(GLFW_KEY_W, Manager, Window);
	AKey = new InputAction(GLFW_KEY_A, Manager, Window);
	SKey = new InputAction(GLFW_KEY_S, Manager, Window);
	DKey = new InputAction(GLFW_KEY_D, Manager, Window);

	Mouse = InputActionMouse(Window);

	WKey->Actions.BindMember(this, &Camera::MoveForward);
	AKey->Actions.BindMember(this, &Camera::MoveLeft);
	SKey->Actions.BindMember(this, &Camera::MoveBackwards);
	DKey->Actions.BindMember(this, &Camera::MoveRight);

	Mouse.BindCallback(this, &Camera::MouseCallback);

	glfwSetInputMode(Window->GetWindow(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	LastX = Window->GetWindowWidth() / 2;
	LastY = Window->GetWindowHeight() / 2;
}

void Camera::MoveForward()
{
	Pos += (Speed * DeltaTime) * Front;
}

void Camera::MoveBackwards()
{
	Pos -= (Speed * DeltaTime) * Front;
}

void Camera::MoveLeft()
{
	Pos -= glm::normalize(glm::cross(Front, Up)) * (Speed * DeltaTime);
}

void Camera::MoveRight()
{
	Pos += glm::normalize(glm::cross(Front, Up)) * (Speed * DeltaTime);
}

void Camera::MouseCallback(float xpos, float ypos)
{
	if (bIsFirstMouse)
	{
		LastX = xpos;
		LastY = ypos;
		bIsFirstMouse = false;
	}

	const float xOffset = (xpos - LastX) * LookSpeed;
	const float yOffset = (LastY - ypos) * LookSpeed;

	LastX = xpos;
	LastY = ypos;

	Yaw += xOffset;
	Pitch += yOffset;

	const float rYaw = glm::radians(Yaw);
	const float rPitch = glm::radians(Pitch);


	if (Pitch > 89)
	{
		Pitch = 89;
	}
	if (Pitch < -89)
	{
		Pitch = -89;
	}

	Directon.x = glm::cos(rYaw) * glm::cos(rPitch);
	Directon.y = glm::sin(rPitch);
	Directon.z = glm::sin(rYaw) * glm::cos(rPitch);
	Front = glm::normalize(Directon);
}

glm::mat4 Camera::GetLook()
{
	glm::vec3 zaxis = glm::normalize(Pos - (Front + Pos));
	// 3. Get positive right axis vector
	glm::vec3 xaxis = glm::normalize(glm::cross(glm::normalize(Up), zaxis));
	// 4. Calculate camera up vector
	glm::vec3 yaxis = glm::cross(zaxis, xaxis);

	// Create translation and rotation matrix
	glm::mat4 translation = glm::mat4(1.0f); // Identity matrix by default
	translation[3][0] = -Pos.x; // Fourth column, first row
	translation[3][1] = -Pos.y;
	translation[3][2] = -Pos.z;
	glm::mat4 rotation = glm::mat4(1.0f);
	rotation[0][0] = xaxis.x; // First column, first row
	rotation[1][0] = xaxis.y;
	rotation[2][0] = xaxis.z;
	rotation[0][1] = yaxis.x; // First column, second row
	rotation[1][1] = yaxis.y;
	rotation[2][1] = yaxis.z;
	rotation[0][2] = zaxis.x; // First column, third row
	rotation[1][2] = zaxis.y;
	rotation[2][2] = zaxis.z;

	return rotation * translation;

	//return glm::lookAt(Pos, Pos + Front, Up);
}
