#pragma once

#include "LinkedList.h"
#include "Verticie.h"

class Shape
{
public:

	virtual void Render() = 0;

	virtual int GetVerticiesSizeByte() = 0;

	virtual float* GetVertices() = 0;

	virtual void SetRenderOrder(LinkedList<int> Indices) = 0;

	virtual LinkedList<Verticie> GetVerts() = 0;

	virtual LinkedList<int> GetIndices() = 0;

};

