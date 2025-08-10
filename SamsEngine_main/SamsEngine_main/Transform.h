#pragma once

#include "Vector3D.h"
#include "glm-1.0.1/glm/glm.hpp"

class Transform
{

public:

	Transform() {};

	Transform(const Vector3D InPosition, const Vector3D InScale, const Vector3D InRotation);

	Transform(const Transform& Copy)
	{
		Position = Copy.Position;
		Scale = Copy.Scale;
		Rotation = Copy.Rotation;
	}

	void CalculateBounds();

	void ReCalculateBounds();

	Vector3D Position;

	Vector3D Scale;

	Vector3D Rotation;

	Vector3D Bounds;

	Vector3D HalfBounds;

	Vector3D Center;

	Vector3D TransCenter;

	Vector3D TransBounds;

	Vector3D TransHalfBounds;

	Vector3D GetRandomPointInBounds();

	float TopHeight;
	float TopWidth;
	float TopLength;
	float BottomHeight;
	float BottomWidth;
	float BottomLength;

	const glm::mat4 GetModelMatrix() const;

};

