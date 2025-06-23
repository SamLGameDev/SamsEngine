#pragma once
#include "Shape.h"
#include "LinkedList.h"
class Verticie;

class Triangle : public Shape
{
public:

	Triangle(Verticie InPos1, Verticie InPos2, Verticie InPos3);
	~Triangle();

	virtual void Render() override;

	virtual int GetVerticiesSizeByte() override
	{
		return sizeof(Vertices);
	}

	virtual float* GetVertices() override
	{
		return Vertices;
	}

	virtual void SetRenderOrder(LinkedList<int> Indices) override;

	virtual LinkedList<Verticie> GetVerts() override
	{
		return Verts;
	}

	virtual LinkedList<int> GetIndices() override
	{
		return IndicesOrder;
	}

private:

	LinkedList<Verticie> Verts;

	LinkedList<int> IndicesOrder;

	float* Vertices;

};

