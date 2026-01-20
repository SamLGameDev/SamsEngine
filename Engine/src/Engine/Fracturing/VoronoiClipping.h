
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
};

class VoronoiClipping
{
public:
	void ClipMeshToVoronoi(const Voronoi& Diagram, const Model& Mesh);
};


