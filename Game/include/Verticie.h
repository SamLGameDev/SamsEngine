// DO NOT MARK.
//This is because it has been submitted for my dissertation. Link to Original: https://github.falmouth.ac.uk/GA-Undergrad-Student-Work-25-26/Dissertation-SL295211.git


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
			Array<Vector3D> set = other.Vertices;

			for (unsigned int i = 0; i < Vertices.GetSize(); i++)
			{
				if (set.Contains(Vertices[i]))
				{
					set.Remove(Vertices[i]);
				}
			}

			if (set.IsEmpty()) return true;

			return false;
		}

		return false;
	}

	bool operator<(const Face& other) const {
		if (other.Vertices.GetSize() == Vertices.GetSize())
		{
			Array<Vector3D> set = other.Vertices;

			for (unsigned int i = 0; i < Vertices.GetSize(); i++)
			{
				if (set.Contains(Vertices[i]))
				{
					set.Remove(Vertices[i]);
				}
			}

			if (set.IsEmpty()) return false;

			return true;
		}

		return true; 
	}
};
