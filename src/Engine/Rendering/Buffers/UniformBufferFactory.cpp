#include "UniformBufferFactory.h"

unsigned int UniformBufferFactory::CreateUniformBuffer(const unsigned int Size, const std::uint8_t location)
{
	unsigned int UBO;
	glGenBuffers(1, &UBO);
	glBindBuffer(GL_UNIFORM_BUFFER, UBO);

	glBufferData(GL_UNIFORM_BUFFER, Size, NULL, GL_STATIC_DRAW);

	glBindBufferBase(GL_UNIFORM_BUFFER, location, UBO);

	glBindBuffer(GL_UNIFORM_BUFFER, 0);
	return UBO;
}

void UniformBufferFactory::SetData(const unsigned int UBO, const unsigned int Start, const unsigned int End, const void* data)
{
	glBindBuffer(GL_UNIFORM_BUFFER, UBO);

	glBufferSubData(GL_UNIFORM_BUFFER, Start, End, data);

	glBindBuffer(GL_UNIFORM_BUFFER, 0);
}