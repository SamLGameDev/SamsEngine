
#pragma once
#include "BaseDataBuffers.h"
#include "ErrorCodes.h"
#include "InitialiseVulkan.h"
#include "Vector2D.h"

class DataBuffers
{
public:

	DataBuffers() = default;
	static void BindVertexInfo(const size_t& ID, const size_t& Location, const size_t& Size, const size_t& Stride, const size_t& Offset, const BufferFormat& Format);

	static void GenBuffer(const size_t& Number, Array<uint32_t>& IDs);
	static void GenBuffer(uint32_t& ID);

	static void GenTexture(const size_t& Number, Array<uint32_t>& IDs);
	static void GenTexture(uint32_t& ID);


	static void BindBuffer(const uint32_t& ID);
	static void BufferData(const uint32_t& ID, const size_t& Size, void* Data, const BufferTargets& Target);

	static void* GenerateUniformDataBuffer(const uint32_t& ID, const size_t& Size);
	static void GenerateShaderStorageBuffer(const uint32_t& ID, const size_t& Size, const size_t& Binding);
	static void BindShaderStorageBuffer(uint32_t ID, const size_t& Binding, const size_t& Size);
	static void* MapBufferMemory(const uint32_t& ID, const size_t& Size);
	static void UnMapBufferMemory(const uint32_t& ID);

	static void RemoveBuffer(const uint32_t& ID);

	static void GenerateDepthBuffer(const uint32_t& ID, const Vector2D& Size);

	static void BufferTexture(const uint32_t& ID, const unsigned char* Pixels, const uint32_t& Width, const uint32_t& Height);

	static BaseDataBuffer* GetBuffer(const uint32_t& ID);
	static BaseDataBuffer* GetTexture(const uint32_t& ID);

	static void DrawVertexData(const uint32_t& ID);

	static void BufferDataIndex(const uint32_t ID, const size_t& Size, void* Data);

	static BaseDataBuffers* APIBufferInstance;
};
