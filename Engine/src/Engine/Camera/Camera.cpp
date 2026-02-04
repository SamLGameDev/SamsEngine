#include "Camera.h"
#include <glm/gtc/matrix_transform.hpp>

#include "DataBuffers.h"
#include "Object.h"
#include "UniformBufferFactory.h"


Camera* Camera::ActiveCamera;

Window* Camera::ActiveWindow;

std::int8_t Camera::ProjectonDir = 1;

void Camera::SetUpInputs(Window* Window, InputManager* Manager)
{
	WKey = std::make_unique<InputAction>(GLFW_KEY_W, Manager, Window);
	AKey = std::make_unique<InputAction>(GLFW_KEY_A, Manager, Window);
	SKey = std::make_unique<InputAction>(GLFW_KEY_S, Manager, Window);
	DKey = std::make_unique<InputAction>(GLFW_KEY_D, Manager, Window);

	Mouse = std::make_unique<InputActionMouse>(Window);

	WKey->Actions.BindMember(this, &Camera::MoveForward);
	AKey->Actions.BindMember(this, &Camera::MoveLeft);
	SKey->Actions.BindMember(this, &Camera::MoveBackwards);
	DKey->Actions.BindMember(this, &Camera::MoveRight);

	Mouse->BindCallback(this, &Camera::MouseCallback);

	AInputManger = Manager;


}

Camera::Camera(Window* Window, InputManager* Manager)
{
	SetUpInputs(Window, Manager);

	glfwSetInputMode(Window->GetWindow(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	//center of the screen
	LastX = Window->GetWindowWidth() / 2;
	LastY = Window->GetWindowHeight() / 2;
}

void Camera::Start()
{
	Object::Start();

	::DataBuffers::GenBuffer(GlobalTransformsID);

	ptr = static_cast<GlobalTransforms*>(::DataBuffers::GenerateUniformDataBuffer(GlobalTransformsID, sizeof(GlobalTransforms)));
}

void Camera::Tick(const double& DeltaTime)
{
	Object::Tick(DeltaTime);

	ptr->Projection = GetProjection();
	ptr->View = GetLook();

	AInputManger->ProcessInput(Camera::GetActiveWindow()->GetWindow());
}

void Camera::MoveForward()
{
	glm::vec3 pos = (Speed * World->GetDeltaTime()) * Front;
	transform.Position += Vector3D(pos.x, pos.y, pos.z);
}

void Camera::MoveBackwards()
{
	glm::vec3 pos = (Speed * World->GetDeltaTime()) * Front;
	transform.Position -= Vector3D(pos.x, pos.y, pos.z);
}

void Camera::MoveLeft()
{
	glm::vec3 pos = glm::normalize(glm::cross(Front, Up)) * (Speed * World->GetDeltaTime());
	transform.Position -= Vector3D(pos.x, pos.y, pos.z);
}

void Camera::MoveRight()
{
	glm::vec3 pos = glm::normalize(glm::cross(Front, Up)) * (Speed * World->GetDeltaTime());
	transform.Position += Vector3D(pos.x, pos.y, pos.z);
}

void Camera::MouseCallback(const double Xpos, const double Ypos)
{
	if (bIsFirstMouse)
	{
		LastX = Xpos;
		LastY = Ypos;
		bIsFirstMouse = false;
	}

	const double xOffset = (Xpos - LastX) * LookSpeed;
	const double yOffset = (LastY - Ypos) * LookSpeed;

	LastX = Xpos;
	LastY = Ypos;

	Yaw += xOffset;
	Pitch += yOffset;

	const float rYaw = static_cast<float>(glm::radians(Yaw));
	const float rPitch = static_cast<float>(glm::radians(Pitch));

	Pitch = std::min<double>(Pitch, 89);

	Pitch = std::max<double>(Pitch, -89);

	Direction.x = glm::cos(rYaw) * glm::cos(rPitch);
	Direction.y = glm::sin(rPitch);
	Direction.z = glm::sin(rYaw) * glm::cos(rPitch);
	Front = glm::normalize(Direction);
}

glm::mat4 Camera::GetLook() const
{
	//Thanks to learn Opengl for this part

	glm::vec3 pos = GetPos();

	const glm::vec3 zAxis = glm::normalize(pos - (Front + pos));

	const glm::vec3 xAxis = glm::normalize(glm::cross(glm::normalize(Up), zAxis));

	const glm::vec3 yAxis = glm::cross(zAxis, xAxis);

	auto translation = glm::mat4(1.0f);
	translation[3][0] = -pos.x; 
	translation[3][1] = -pos.y;
	translation[3][2] = -pos.z;

	auto rotation = glm::mat4(1.0f);
	rotation[0][0] = xAxis.x; 
	rotation[1][0] = xAxis.y;
	rotation[2][0] = xAxis.z;
	rotation[0][1] = yAxis.x; 
	rotation[1][1] = yAxis.y;
	rotation[2][1] = yAxis.z;
	rotation[0][2] = zAxis.x;
	rotation[1][2] = zAxis.y;
	rotation[2][2] = zAxis.z;

	return rotation * translation;
}

glm::mat4 Camera::GetProjection()
{
	glm::mat4 projection = glm::perspective<float>
	(
		glm::radians(FOV),
		static_cast<float>(ActiveWindow->GetWindowWidth()) / static_cast<float>(ActiveWindow->GetWindowHeight()),
		NearView,
		FarView
	);

	//Reverse for vulkan
	//TODO make this more part of vulkan, so it doesnt interfere with OpenGL
	projection[1][1] *= ProjectonDir;
	return projection;
}
