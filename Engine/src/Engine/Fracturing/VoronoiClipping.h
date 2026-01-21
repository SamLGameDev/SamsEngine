
#pragma once

#include "Voronoi.h"


struct FTriangle
{
	Vector3D Verts[3];

	FTriangle() = default;

	FTriangle(const Vector3D& A, const Vector3D& B, const Vector3D& C)
	{
		Verts[0] = A;
		Verts[1] = B;
		Verts[2] = C;
	}

	Vector3D& operator[](const size_t& Index)
	{
		if (Index > 2)
		{
			throw std::runtime_error("Triangle out of index");
		}

		return Verts[Index];
	}

	Vector3D& begin() { return Verts[0]; }
	Vector3D& end() { return Verts[2]; }
};

class VoronoiClipping
{
public:
	void ClipMeshToVoronoi(const Voronoi& Diagram, const Model& Mesh);
};


