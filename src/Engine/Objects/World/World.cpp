#include "World.h"
#include "Rendering/Buffers/UniformBufferFactory.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
void World::Tick(const double& InDeltaTime)
{
	Object::Tick(DeltaTime);

	DeltaTime = InDeltaTime;

	*TimeBufferPtr = glfwGetTime();
}

void World::Start()
{
	Object::Start();

	TimeBufferPtr = UniformBufferFactory::CreatePersistentUniformBuffer<double>(1);
}
