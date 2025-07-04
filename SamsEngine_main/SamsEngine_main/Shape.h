#pragma once

#include "LinkedList.h"
#include "Verticie.h"
#include "Vector3D.h"
#include "Vector2D.h"

class Shape
{
public:

	virtual void Render() = 0;

	virtual int GetVerticiesSizeByte() = 0;

	virtual float* GetVertices() = 0;

	virtual void SetRenderOrder(LinkedList<int> Indices) = 0;

	virtual LinkedList<Vertex> GetVerts() = 0;

	virtual LinkedList<int> GetIndices() = 0;

	const virtual Vector3D GetColor() const
	{
		return Color;
	};

	const virtual LinkedList<Vector2D>& GetTextureCords()const
	{
		return TextureCords;
	};

	const virtual unsigned int GetTexture(const int index)const
	{
		return Textures[index];
	};

protected:

	Vector3D Color;

	LinkedList<Vector2D> TextureCords;

	LinkedList<unsigned int> Textures;
};

