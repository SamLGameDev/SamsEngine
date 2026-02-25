
#pragma once
#include "ErrorCodes.h"
#include "InitialiseVulkan.h"
#include "Vector2D.h"

enum BufferTargets : uint8_t
{
	VERTEX,
	Indices,
	Transfer
};

enum BufferFormat : uint8_t
{
	Vector2,
	Vector3,
};

class BaseDataBuffer
{
public:

	virtual ~BaseDataBuffer() = default;
};



class BaseDataBuffers
{
public:

	BaseDataBuffers() = default;
	virtual ~BaseDataBuffers() = default;

	virtual void BindVertexInfo(const uint32_t& ID, const size_t& Location, const size_t& Size, const size_t& Stride, const size_t& Offset, const
	                            BufferFormat& Format) = 0;
	virtual void GenBuffer(const size_t& Number, Array<uint32_t>& IDs) = 0;
	virtual void GenBuffer(uint32_t& ID) = 0;

	virtual void GenTexture(const size_t& Number, Array<uint32_t>& IDs) = 0;
	virtual void GenTexture(uint32_t& ID) = 0;

	virtual void BindBuffer(const uint32_t& ID) = 0;

	virtual void BufferData(const uint32_t& ID, const size_t& Size, void* Data, const BufferTargets& Target) = 0;

	virtual void BufferDataIndex(const uint32_t ID, const size_t& Size, void* Data) = 0;

	virtual void* GenerateUniformDataBuffer(const uint32_t ID, const size_t& Size) = 0;

	virtual void GenerateShaderStorageBuffer(const uint32_t ID, const size_t& Size, const size_t& Binding) = 0;

	virtual void BindShaderStorageBuffer(uint32_t ID, const size_t& Binding, const size_t& Size) = 0;

	virtual void* MapBufferMemory(const uint32_t& ID, const size_t& Size) = 0;
	virtual void UnMapBufferMemory(const uint32_t& ID) = 0;

	virtual void RemoveBuffer(const uint32_t& ID) = 0;

	virtual void GenerateDepthBuffer(const uint32_t& ID, const Vector2D& Size) = 0;

	virtual void BufferTexture(const uint32_t& ID, const unsigned char* Pixels, const uint32_t& Width, const uint32_t& Height) = 0;


	
	virtual BaseDataBuffer* GetBuffer(const uint32_t& ID) = 0;
	virtual BaseDataBuffer* GetTexture(const uint32_t& ID) = 0;

	virtual void DrawVertexData(const uint32_t& ID) = 0;
};
