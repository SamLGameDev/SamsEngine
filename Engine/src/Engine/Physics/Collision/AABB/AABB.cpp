#include "AABB.h"

Array<Face> FBox::GetFaces() const
{
	Array<Face> Faces;
	Faces.Add(Face{ { Vector3D(min.X, min.Y, min.Z), Vector3D(max.X, min.Y, min.Z), Vector3D(max.X, max.Y, min.Z), Vector3D(min.X, max.Y, min.Z) }}); // Back Face
	Faces.Add(Face{ { Vector3D(min.X, min.Y, max.Z), Vector3D(max.X, min.Y, max.Z), Vector3D(max.X, max.Y, max.Z), Vector3D(min.X, max.Y, max.Z) } }); // Front Face
	Faces.Add(Face{ { Vector3D(min.X, min.Y, min.Z), Vector3D(min.X, max.Y, min.Z), Vector3D(min.X, max.Y, max.Z), Vector3D(min.X, min.Y, max.Z) } }); // Left Face
	Faces.Add(Face{ { Vector3D(max.X, min.Y, min.Z), Vector3D(max.X, max.Y, min.Z), Vector3D(max.X, max.Y, max.Z), Vector3D(max.X, min.Y, max.Z) } }); // Right Face
	Faces.Add(Face{ { Vector3D(min.X, min.Y, min.Z), Vector3D(max.X, min.Y, min.Z), Vector3D(max.X, min.Y, max.Z), Vector3D(min.X, min.Y, max.Z) } }); // Bottom Face
	Faces.Add(Face{ { Vector3D(min.X, max.Y, min.Z), Vector3D(max.X, max.Y, min.Z), Vector3D(max.X, max.Y, max.Z), Vector3D(min.X, max.Y, max.Z) } }); // Top Face
	return Faces;
}

bool AABB::IsPointInsideBox(const FBox& Box, const Vector3D& Point)
{
	return Point >= Box.min && Point <= Box.max;
}

bool AABB::IsBoxIntersectingBox(const FBox& Box1, const FBox& Box2)
{
	return Box1.min <= Box2.max && Box1.max >= Box2.min;
}
