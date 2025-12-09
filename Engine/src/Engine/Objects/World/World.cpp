#include "World.h"
#include "Rendering/Buffers/UniformBufferFactory.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
void World::Tick(const double& InDeltaTime)
{
	Object::Tick(DeltaTime);

	DeltaTime = InDeltaTime;
}

void World::Start()
{
	Object::Start();

}
