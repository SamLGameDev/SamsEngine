#include "Transform.h"
#include <glm/gtc/matrix_transform.hpp>

Transform::Transform(const Vector3D& InPosition, const Vector3D& InScale, const Vector3D& InRotation)
{
	Position = InPosition;

	Scale = InScale;

	Rotation = InRotation;
}

void Transform::CalculateBounds()
{
	//Get the length of the bounds on each axis
	Bounds = HighestPoints.Abs() + LowestPoints.Abs();

	glm::vec4 bounds = GetModelMatrix() * glm::vec4(Bounds.X, Bounds.Y, Bounds.Z, 1);

	TransBounds = Vector3D(bounds.x, bounds.y, bounds.z);

	HalfBounds = Bounds / 2;

	TransHalfBounds = TransBounds / 2;

	Center = (HighestPoints + LowestPoints) / 2;

	//need to convert to glm here, for matric transformations.
	//TODO create my own matrix class to replace this
	const glm::vec4 center = glm::vec4(Center.X, Center.Y, Center.Z,1);

	const glm::vec4 transCenter = GetModelMatrix() * center;

	TransCenter = Vector3D(transCenter.x, transCenter.y, transCenter.z);
}


Vector3D Transform::GetRandomPointInBounds() const
{
	const Vector3D min = TransCenter - TransHalfBounds;

	const Vector3D max = TransCenter + TransHalfBounds;

	return Vector3D::RandomRange(min, max);
}

glm::mat4 Transform::GetModelMatrix() const
{
	glm::mat4 model = glm::mat4(1);

	model = glm::translate(model, glm::vec3(Position.X, Position.Y, Position.Z));

	model = glm::rotate(model, glm::radians(Rotation.Z), glm::vec3(0, 0, 1));
	model = glm::rotate(model, glm::radians(Rotation.Y), glm::vec3(0, 1, 0));
	model = glm::rotate(model, glm::radians(Rotation.X), glm::vec3(1, 0, 0));

	model = glm::scale(model, glm::vec3(Scale.X, Scale.Y, Scale.Z));

	return model;
}

glm::mat4 Transform::GetInverseModelMatrix() const
{
	return glm::inverse(GetModelMatrix());
}