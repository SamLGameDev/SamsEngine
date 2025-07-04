#pragma once
#include "Shape.h"
#include "LinkedList.h"
#include "Vector3D.h"
#include "Vector2D.h"
class Vertex;

class Triangle : public Shape
{
public:

	Triangle(Vertex InPos1, Vertex InPos2, Vertex InPos3, Vector3D color = Vector3D(0,0,0), Vector2D TexPos1 = Vector2D(0, 0), Vector2D TexPos2 = Vector2D(0, 0), Vector2D TexPos3 = Vector2D(0, 0));
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

	virtual LinkedList<Vertex> GetVerts() override
	{
		return Verts;
	}

	virtual LinkedList<int> GetIndices() override
	{
		return IndicesOrder;
	}

private:

	LinkedList<Vertex> Verts;

	LinkedList<int> IndicesOrder;

	float* Vertices;

};

