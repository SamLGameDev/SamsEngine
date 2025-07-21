#include "Shape.h"


Shape::Shape()
{
}

void Shape::Draw(glm::mat4 Translation, glm::mat4 Model, glm::mat4 View, glm::mat4 Projection)
{
	shader.Use();

	shader.SetMatrix4fv("Transform", glm::value_ptr(Translation));

	shader.SetMatrix4fv("Model", glm::value_ptr(Model));

	shader.SetMatrix4fv("View", glm::value_ptr(View));

	shader.SetMatrix4fv("Projection", glm::value_ptr(Projection));

	shader.ApplyTextures();
}