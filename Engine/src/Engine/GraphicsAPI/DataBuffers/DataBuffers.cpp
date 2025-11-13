#include "DataBuffers.h"
#include<glad/glad.h>
#include "GLFW/glfw3.h"


BaseDataBuffers* DataBuffers::APIBufferInstance;

void DataBuffers::BindVertexInfo(const size_t& ID, const size_t& Location, const size_t& Size, const size_t& Stride,
	const size_t& Offset)
{
	APIBufferInstance->BindVertexInfo(ID, Location, Size, Stride, Offset);
}

void DataBuffers::GenBuffer(const size_t& Number, Array<uint32_t>& IDs)
{
	APIBufferInstance->GenBuffer(Number, IDs);
}

void DataBuffers::GenBuffer(uint32_t& ID)
{
	APIBufferInstance->GenBuffer(ID);
}

void DataBuffers::BindBuffer(const uint32_t& ID)
{
	APIBufferInstance->BindBuffer(ID);
}

void DataBuffers::BufferData(const uint32_t& ID, const size_t& Size, void* Data, const BufferTargets& Target)
{
	APIBufferInstance->BufferData(ID, Size, Data, Target);
}

void* DataBuffers::GenerateUniformDataBuffer(const uint32_t ID, const size_t& Size)
{
	return APIBufferInstance->GenerateUniformDataBuffer(ID, Size);
}

BaseDataBuffer* DataBuffers::GetBuffer(const uint32_t& ID)
{
	return APIBufferInstance->GetBuffer(ID);
}

void DataBuffers::DrawVertexData(const uint32_t& ID)
{
	APIBufferInstance->DrawVertexData(ID);
}

void DataBuffers::BufferDataIndex(const uint32_t ID, const size_t& Size, void* Data)
{
	APIBufferInstance->BufferDataIndex(ID, Size, Data);
}

