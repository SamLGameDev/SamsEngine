#pragma once
#include <glad/glad.h>

#include <cstdint>

class UniformBufferFactory
{
public:


	/**
	 * @param Size How big you want the buffer to be, think Storage Item sizeof * amount
	 * @param location The global shader buffer location
	 * @return A uniform buffer ID
	 */
	static unsigned int CreateUniformBuffer(unsigned int Size, std::uint8_t location);



	/**
	 * A persistent buffer
	 * @tparam T The type of object the persistent buffer houses, like a float, or custom struct
	 * @param location The global shader buffer location
	 * @return A ptr to the buffer
	 */
	template<typename T>
	[[nodiscard]] static T* CreatePersistentUniformBuffer(std::uint8_t location);


	/**
	 * Write Info to a uniform buffer, do not use for persistent buffer
	 * @param UBO The ID of the uniform Buffer
	 * @param Start From where in the buffer should we start to write. if size is 64, we could start writing at 32
	 * @param End Where should we stop writing
	 * @param data The data to write to the buffer
	 */
	static void SetData(unsigned int UBO, unsigned int Start, unsigned int End, const void* data);
};

template<typename T>
T* UniformBufferFactory::CreatePersistentUniformBuffer(const std::uint8_t location)
{
	const GLsizeiptr size = sizeof(T);

	unsigned int UBO;
	glGenBuffers(1, &UBO);
	glBindBuffer(GL_UNIFORM_BUFFER, UBO);

	constexpr GLenum flags = GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT;

	glBindBufferBase(GL_UNIFORM_BUFFER, location, UBO);

	glBufferStorage(GL_UNIFORM_BUFFER, size, NULL, flags);

	T* ptr = static_cast<T*>(glMapBufferRange(GL_UNIFORM_BUFFER, 0, size, flags));

	glBindBuffer(GL_UNIFORM_BUFFER, 0);
	return ptr;
}
