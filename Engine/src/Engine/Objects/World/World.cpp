// DO NOT MARK 
//This is because it has been submitted for my COMP305. Link to Original: https://github.falmouth.ac.uk/GA-Undergrad-Student-Work-25-26/Comp305-Engine-SL295211.git
#include "World.h"
#include "Rendering/Buffers/UniformBufferFactory.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
void World::Tick(const double& InDeltaTime)
{
	Object::Tick(DeltaTime);

	DeltaTime = static_cast<float>(InDeltaTime);
}

void World::Start()
{
	Object::Start();

}
