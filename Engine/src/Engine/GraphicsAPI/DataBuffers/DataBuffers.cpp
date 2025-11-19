#include "DataBuffers.h"
#include<glad/glad.h>
#include "GLFW/glfw3.h"


BaseDataBuffers* DataBuffers::APIBufferInstance;

void DataBuffers::BindVertexInfo(const size_t& ID, const size_t& Location, const size_t& Size, const size_t& Stride,
	const size_t& Offset, const BufferFormat& Format)
{
	APIBufferInstance->BindVertexInfo(ID, Location, Size, Stride, Offset, Format);
}

void DataBuffers::GenBuffer(const size_t& Number, Array<uint32_t>& IDs)
{
	APIBufferInstance->GenBuffer(Number, IDs);
}

void DataBuffers::GenBuffer(uint32_t& ID)
{
	APIBufferInstance->GenBuffer(ID);
}

void DataBuffers::GenTexture(const size_t& Number, Array<uint32_t>& IDs)
{
	APIBufferInstance->GenTexture(Number, IDs);
}

void DataBuffers::GenTexture(uint32_t& ID)
{
	APIBufferInstance->GenTexture(ID);
}

void DataBuffers::BindBuffer(const uint32_t& ID)
{
	APIBufferInstance->BindBuffer(ID);
}

void DataBuffers::BufferData(const uint32_t& ID, const size_t& Size, void* Data, const BufferTargets& Target)
{
	APIBufferInstance->BufferData(ID, Size, Data, Target);
}

void* DataBuffers::GenerateUniformDataBuffer(const uint32_t& ID, const size_t& Size)
{
	return APIBufferInstance->GenerateUniformDataBuffer(ID, Size);
}

void DataBuffers::BufferTexture(const uint32_t& ID, const unsigned char* Pixels, const uint32_t& Width, const uint32_t& Height)
{
	APIBufferInstance->BufferTexture(ID, Pixels, Width, Height);
}

BaseDataBuffer* DataBuffers::GetBuffer(const uint32_t& ID)
{
	return APIBufferInstance->GetBuffer(ID);
}

BaseDataBuffer* DataBuffers::GetTexture(const uint32_t& ID)
{
	return APIBufferInstance->GetTexture(ID);
}

void DataBuffers::DrawVertexData(const uint32_t& ID)
{
	APIBufferInstance->DrawVertexData(ID);
}

void DataBuffers::BufferDataIndex(const uint32_t ID, const size_t& Size, void* Data)
{
	APIBufferInstance->BufferDataIndex(ID, Size, Data);
}
void DataBuffers::GenerateDepthBuffer(const uint32_t& ID, const Vector2D& Size)
{
	APIBufferInstance->GenerateDepthBuffer(ID, Size);
}
