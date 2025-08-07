#include "Transform.h"
#include "glm-1.0.1/glm/gtc/matrix_transform.hpp"
#include <iostream>

Transform::Transform(const Vector3D InPosition, const Vector3D InScale, const Vector3D InRotation)
{
	Position = InPosition;

	Scale = InScale;

	Rotation = InRotation;

	TopWidth = -std::numeric_limits<float>::max();
	BottomWidth = std::numeric_limits<float>::max();

	TopHeight = -std::numeric_limits<float>::max();
	BottomHeight = std::numeric_limits<float>::max();

	TopLength = -std::numeric_limits<float>::max();
	BottomLength = std::numeric_limits<float>::max();

}

void Transform::CalculateBounds()
{
	Bounds.X = std::abs(TopWidth) + std::abs(BottomWidth);
	Bounds.Y = std::abs(TopHeight) + std::abs(BottomHeight);
	Bounds.Z = std::abs(TopLength) + std::abs(BottomLength);

	HalfBounds = Bounds / 2;

	glm::vec4 center = glm::vec4((TopWidth + BottomWidth) / 2, (TopHeight + BottomHeight) /2, (TopLength + BottomLength) /2, 1);

	center = GetModelMatrix() * center;

	Center = Vector3D(center.x, center.y, center.z);

	std::cout << Center.X << Center.Y << Center.Z << std::endl;
}

const glm::mat4 Transform::GetModelMatrix() const
{
	glm::mat4 model = glm::mat4(1);
	model = glm::translate(model, glm::vec3(Position.X, Position.Y, Position.Z));

	model = glm::rotate(model, glm::radians(Rotation.X), glm::vec3(1, 0, 0));
	model = glm::rotate(model, glm::radians(Rotation.Y), glm::vec3(0, 1, 0));
	model = glm::rotate(model, glm::radians(Rotation.Z), glm::vec3(0, 0, 1));

	model = glm::scale(model, glm::vec3(Scale.X, Scale.Y, Scale.Z));

	return model;
}
