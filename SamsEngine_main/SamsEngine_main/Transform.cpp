#include "Transform.h"
#include "glm-1.0.1/glm/gtc/matrix_transform.hpp"

Transform::Transform(const Vector3D InPosition, const Vector3D InScale, const Vector3D InRotation)
{
	Position = InPosition;

	Scale = InScale;

	Rotation = InRotation;
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
