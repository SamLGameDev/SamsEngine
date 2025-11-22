#pragma once
#include "Vector3D.h"
#include "Vector2D.h"
#include "Array.h"
#include <functional>
struct Vector3D;

struct Vertex
{
public:

	Vector3D Position;

	Vector3D Normal;

	Vector2D TexCoords;

	Vector3D Color;

	[[nodiscard]] bool operator==(const Vertex& other) const {
		return Position == other.Position;
	}

	Vertex(const Vector3D& pos, const Vector3D& norm, const Vector2D& tex, const Vector3D& col) :
	Position(pos), Normal(norm), TexCoords(tex), Color(col) {}

	Vertex(const Vector3D& pos, const Vector3D& col) : Position(pos), Color(col) {}

	Vertex() = default;
};

struct Face
{
public:

	Array<Vector3D> Vertices;

	bool operator==(const Face& other) const {
		if (other.Vertices.GetSize() == Vertices.GetSize())
		{
			for (unsigned int i = 0; i < Vertices.GetSize(); i++)
			{
				if (Vertices[i] != other.Vertices[i])
				{
					return false;
				}
			}
			return true;
		}

		return false;
	}
};
