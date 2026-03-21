
#pragma once
#include <map>

#include "BaseDataBuffers.h"
#include "ErrorCodes.h"
#include "InitialiseVulkan.h"
#include "glad/glad.h"

namespace OpenGL
{
	class DataBuffer : public BaseDataBuffer
	{
	public:
		std::optional<GLuint> VAO;
		Array<GLuint> VBO;
		std::optional<GLuint> EBO;

		GLuint UBO;

		GLintptr CurrentOffset = 0;

		GLsizei IndsSize = 0;

		~DataBuffer() override;

		DataBuffer() = default;
	};

	class DataBuffers : public BaseDataBuffers
	{
	public:

		DataBuffers() = default;

		virtual void BindVertexInfo(const uint32_t& ID, const size_t& Location, const size_t& Size, const size_t& Stride, const size_t& Offset, const
			BufferFormat& Format) override;
		virtual void GenBuffer(const size_t& Number, Array<uint32_t>& IDs) override;
		virtual void GenBuffer(uint32_t& ID)  override;

		virtual void GenTexture(const size_t& Number, Array<uint32_t>& IDs)  override;
		virtual void GenTexture(uint32_t& ID)  override;

		virtual void BindBuffer(const uint32_t& ID)  override;

		virtual void BufferData(const uint32_t& ID, const size_t& Size, void* Data, const BufferTargets& Target) override;

		virtual void BufferDataIndex(const uint32_t ID, const size_t& Size, void* Data)  override;

		virtual void* GenerateUniformDataBuffer(const uint32_t ID, const size_t& Size) override;
		void GenerateShaderStorageBuffer(const uint32_t ID, const size_t& Size, const size_t& Binding) override;
		void BindShaderStorageBuffer(uint32_t ID, const size_t& Binding, const size_t& Size) override;
		void* MapBufferMemory(const uint32_t& ID, const size_t& Size) override;
		void UnMapBufferMemory(const uint32_t& ID) override;

		void RemoveBuffer(const uint32_t& ID) override;

		virtual void GenerateDepthBuffer(const uint32_t& ID, const Vector2D& Size) override;

		virtual void BufferTexture(const uint32_t& ID, const unsigned char* Pixels, const uint32_t& Width, const uint32_t& Height) override;



		virtual BaseDataBuffer* GetBuffer(const uint32_t& ID)  override;
		virtual BaseDataBuffer* GetTexture(const uint32_t& ID) override;

		virtual void DrawVertexData(const uint32_t& ID)  override;

		std::map<uint32_t, DataBuffer> RegisteredBuffers;
	};

}
