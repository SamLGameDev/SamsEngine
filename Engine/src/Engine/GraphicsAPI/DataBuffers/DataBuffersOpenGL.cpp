#include "DataBuffersOpenGL.h"

#include <cassert>
#include <iostream>
#include<glad/glad.h>

#include "MathCore.h"
#include "Vector3D.h"
#include "GLFW/glfw3.h"

namespace OpenGL
{
	DataBuffer::~DataBuffer()
	{

	}

	void DataBuffers::BindVertexInfo(const uint32_t& ID, const size_t& Location, const size_t& Size,
	                                 const size_t& Stride, const size_t& Offset, const BufferFormat& Format)
	{

		DataBuffer& buffer = RegisteredBuffers[ID];

		if (!buffer.VAO.has_value() )
		{
			GLuint VAO;
			GLuint EBO;

			glGenVertexArrays(1, &VAO);

			glGenBuffers(1, &EBO);

			buffer.VAO = VAO;

			buffer.EBO = EBO;

		}
		GLuint VBO;
		glGenBuffers(1, &VBO);
		buffer.VBO.Add(VBO);

		glBindVertexArray(buffer.VAO.value());

		glBindBuffer(GL_ARRAY_BUFFER, *buffer.VBO.GetLastPtr());

		GLint length = Format == BufferFormat::Vector3 ? 3 : 2;

		glEnableVertexAttribArray(Location);
		glVertexAttribPointer(Location, length, GL_FLOAT, GL_FALSE, Stride, static_cast<void*>(0));
		glBindVertexArray(0);

	}

	void DataBuffers::GenBuffer(const size_t& Number, Array<uint32_t>& IDs)
	{
	}

	void DataBuffers::GenBuffer(uint32_t& ID)
	{
		do
		{
			ID = MathCore::RandomRange<uint32_t>(0, std::numeric_limits<uint32_t>::max());
		} while (RegisteredBuffers.contains(ID));
		RegisteredBuffers.insert({ ID, DataBuffer() });
	}

	void DataBuffers::GenTexture(const size_t& Number, Array<uint32_t>& IDs)
	{
	}

	void DataBuffers::GenTexture(uint32_t& ID)
	{
	}

	void DataBuffers::BindBuffer(const uint32_t& ID)
	{
		DataBuffer& buffer = RegisteredBuffers[ID];

		glBindVertexArray(buffer.VAO.value());
	}

	void DataBuffers::BufferData(const uint32_t& ID, const size_t& Size, void* Data, const BufferTargets& Target)
	{
		DataBuffer& buffer = RegisteredBuffers[ID];

		glBindVertexArray(buffer.VAO.value());

		glBindBuffer(GL_ARRAY_BUFFER, *buffer.VBO.GetLastPtr());

		glBufferData(GL_ARRAY_BUFFER, Size, Data, GL_STATIC_DRAW);


	}

	void DataBuffers::BufferDataIndex(const uint32_t ID, const size_t& Size, void* Data)
	{
		DataBuffer& buffer = RegisteredBuffers[ID];

		glBindVertexArray(buffer.VAO.value());

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer.EBO.value());
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, Size, Data, GL_STATIC_DRAW);

		buffer.IndsSize = Size;

	}

	void* DataBuffers::GenerateUniformDataBuffer(const uint32_t ID, const size_t& Size)
	{

		DataBuffer& buffer = RegisteredBuffers[ID];

		GLuint UBO = 0;
		GLenum error = glGetError();
		if (error != GL_NO_ERROR)
		{
			std::cout << "ERROR::UNIFORMBUFFER::" << std::to_string(error) << std::endl;
		}
		glGenBuffers(1, &UBO);


		glBindBuffer(GL_UNIFORM_BUFFER, UBO);


		constexpr GLenum flags = GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT;
		assert(glGetError() == GL_NO_ERROR);
		glBufferStorage(GL_UNIFORM_BUFFER, Size, NULL, flags);
		assert(glGetError() == GL_NO_ERROR);
		void* ptr = glMapBufferRange(GL_UNIFORM_BUFFER, 0, Size, flags);
		error = glGetError();
		if (error != GL_NO_ERROR)
		{
			std::cout << "ERROR::UNIFORMBUFFER::" << std::to_string(error) << std::endl;
		}

		glBindBuffer(GL_UNIFORM_BUFFER, 0);
		buffer.UBO = UBO;
		return ptr;
	}

	void DataBuffers::GenerateDepthBuffer(const uint32_t& ID, const Vector2D& Size)
	{
	}

	void DataBuffers::BufferTexture(const uint32_t& ID, const unsigned char* Pixels, const uint32_t& Width,
		const uint32_t& Height)
	{
	}

	BaseDataBuffer* DataBuffers::GetBuffer(const uint32_t& ID)
	{
		return &RegisteredBuffers[ID];
	}

	BaseDataBuffer* DataBuffers::GetTexture(const uint32_t& ID)
	{
		return nullptr;
	}

	void DataBuffers::DrawVertexData(const uint32_t& ID)
	{
		DataBuffer& buffer = RegisteredBuffers[ID];
	}
}
