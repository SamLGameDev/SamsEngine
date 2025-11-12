
#pragma once
#include "ErrorCodes.h"
#include "InitialiseVulkan.h"

enum BufferTargets : uint8_t
{
	Vertex,
	Indices,
	Transfer
};


class BaseDataBuffers
{
public:

	BaseDataBuffers() = default;
	virtual ~BaseDataBuffers() = default;

	virtual void BindVertexInfo(const uint32_t& ID, const size_t& Location, const size_t& Size, const size_t& Stride, const size_t& Offset) = 0;
	virtual void GenBuffer(const size_t& Number, Array<uint32_t>& IDs) = 0;
	virtual void GenBuffer(uint32_t& ID) = 0;

	virtual void BindBuffer(const uint32_t& ID) = 0;

	virtual void BufferData(const uint32_t& ID, const size_t& Size, void* Data, const BufferTargets& Target) = 0;

	virtual void BufferDataIndex(const uint32_t ID, const size_t& Size, void* Data) = 0;

	virtual void DrawVertexData(const uint32_t& ID) = 0;
};

class BaseDataBuffer
{
	
};

