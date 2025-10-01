#pragma once

#include <glm/glm.hpp>
#include "FirstWindow.h"
#include "InputManager.h"
#include "InputAction.h"
#include "InputActionMouse.h"
#include "WorldObject.h"
#include <memory>

class Camera final : public WorldObject
{
public:
	void SetUpInputs(FirstWindow* Window, InputManager* Manager);
	Camera(FirstWindow* Window, InputManager* Manager);

	void Start() override;

	void Tick(const double& DeltaTime) override;

	void MoveForward();

	void MoveBackwards();

	void MoveLeft();

	void MoveRight();

	void MouseCallback(double Xpos, double Ypos);

	[[nodiscard]] glm::vec3 GetPos() const
	{
		return Pos;
	}


	/**
	 * @return The view matrix of the camera
	 */
	[[nodiscard]] glm::mat4 GetLook() const;

	/**
	 * @return The camera currently being used
	 */
	static Camera* GetActiveCamera()
	{
		return ActiveCamera;
	};

	static void SetActiveCamera(Camera* NewCamera)
	{
		ActiveCamera = NewCamera;
	}

	[[nodiscard]] static FirstWindow* GetActiveWindow()
	{
		return ActiveWindow;
	};

	static void SetActiveWindow(FirstWindow* NewWindow)
	{
		ActiveWindow = NewWindow;
	}


	/**
	 * @return The projection matrix of the camera
	 */
	[[nodiscard]] static glm::mat4 GetProjection();


	/**
	 * @return The Current facing direction of the camera
	 */
	[[nodiscard]] glm::vec3 GetFront() const
	{
		return Front;
	}

	struct Transforms
	{
		glm::mat4 View;
		glm::mat4 Projection;
	};

private:

	double Yaw = -90;
	double Pitch = 0;

	bool bIsFirstMouse = true;

	double LastX, LastY;

	constexpr static float Speed = 3.0f;

	constexpr static float LookSpeed = 0.1f;

	constexpr static float FOV = 45.0f;
	constexpr static float NearView = 0.1f;
	constexpr static float FarView = 100.f;

	std::unique_ptr<InputAction> WKey;
	std::unique_ptr<InputAction> AKey;
	std::unique_ptr<InputAction> SKey;
	std::unique_ptr<InputAction> DKey;

	std::unique_ptr<InputActionMouse> Mouse;

	glm::vec3 Pos = glm::vec3(0, 0, 3);
	glm::vec3 Front = glm::vec3(0, 0, -1);
	glm::vec3 Up = glm::vec3(0, 1, 0);

	glm::vec3 Direction = glm::vec3(0, 0, 0);

	static Camera* ActiveCamera;

	static FirstWindow* ActiveWindow;

	Transforms* ptr;
};
