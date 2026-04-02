

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

	Vector3D Normal;

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

	Vector3D GetCenter() const
	{
		Vector3D center = Vector3D::Zero;
		for (const auto& vert : Vertices)
		{
			center += vert;
		}
		return center / static_cast<float>(Vertices.GetSize());
	}
};
struct Edge
{
	Vector3D P1;
	Vector3D P2;
	Edge(const Vector3D& InP1, const Vector3D& InP2) : P1(InP1), P2(InP2) {};

	Edge() : P1(Vector3D::Zero), P2(Vector3D::Zero) {};

	bool operator==(const Edge& other) const
	{
		return (P1 == other.P1 && P2 == other.P2) || (P1 == other.P2 && P2 == other.P1);
	}

};
struct Tetrahedron
{
	Vector3D point1;
	Vector3D point2;
	Vector3D point3;
	Vector3D point4;

	Face faces[4];

	bool IsPointInCircumSphere(const Vector3D& Point) const;

	Tetrahedron(const Vector3D& InP1, const Vector3D& InP2, const Vector3D& InP3, const Vector3D& InP4) :
		point1(InP1), point2(InP2), point3(InP3), point4(InP4)
	{
		faces[0] = { {InP1, InP2, InP3} };
		faces[1] = { {InP1, InP2, InP4} };
		faces[2] = { {InP1, InP3, InP4} };
		faces[3] = { {InP2, InP3, InP4} };
	};

	Vector3D operator[](const size_t& Index) const
	{
		if (Index == 0) return point1;
		if (Index == 1) return point2;
		if (Index == 2) return point3;
		if (Index == 3) return point4;
		throw std::runtime_error("Tetrahedron out of index");
	}

	bool ContainsPoint(const Vector3D& Point) const
	{
		for (size_t i = 0; i < 4; i++)
		{
			if ((*this)[i] == Point)
			{
				return true;
			}
		}
		return false;
	}

	Tetrahedron() = default;
};
