
#include "PlaneClipping.h"

#include <iostream>

#include "MathCore.h"

void PlaneClipping::ClipCellByFaces(Array<Face>& ToClip, const Array<Face>& ClippingPlanes)
{
	Vector3D center = Vector3D::Zero;
	for (const auto& face : ClippingPlanes)
	{
		center += face.GetCenter();
	}
	center = center / ClippingPlanes.GetSize();

	for (const auto& plane : ClippingPlanes)
	{
		if (ToClip.IsEmpty()) return;

		Vector3D normal = Vector3D::GetPlaneNormal(plane.Vertices, center);

		if (!MathCore::IsNearlyZero(Vector3D::Dot(normal, center - plane.GetCenter())) &&
			Vector3D::Dot(normal, center - plane.GetCenter()) < 0)
		{
			normal = -normal;
		}

		if (Vector3D::IsAlmostEqual(normal, Vector3D::Zero))
		{
			std::cout << "Plane normal is zero, skipping clipping for this plane." << std::endl; continue;
		}
		ClipCellByFace(ToClip, plane.GetCenter(), normal);

		for (auto& face : ToClip)
		{
			Vector3D::OrderByAngle(face.Vertices, face.GetCenter(), normal);
		}
	}
}

void PlaneClipping::ClipCellByFace(Array<Face>& ToClip, const Vector3D& Center, const Vector3D& normal)
{
	Face intersectFace;

	Array<Face> newFaces;

	for (const auto& face : ToClip)
	{

		Face outFace;
		ClipFaceByFace(face, Center, outFace, normal, intersectFace);
		if (!outFace.Vertices.IsEmpty())
		{
			newFaces.Add(outFace);
		}
	}

	if (intersectFace.Vertices.GetSize() >= 3)
	{
		Vector3D::OrderByAngle(intersectFace.Vertices, Center, normal);

		newFaces.Add(intersectFace);
	}

	ToClip = newFaces;
}
void PlaneClipping::ClipFaceByFace(const Face& ToClip, const Vector3D& Center, Face& OutFace, const Vector3D& Normal, Face& IntersectFace)
{
	size_t firstIntersectionIndex = 0;
	Vector3D firstIntersection;

	GetFirstIntersection(Normal, Center, ToClip, OutFace, firstIntersectionIndex, firstIntersection);

	if (OutFace.Vertices.IsEmpty())
	{
		if (Vector3D::Dot(Normal, ToClip.Vertices[0] - Center) > 0)
		{
			OutFace = ToClip;
		}
		return;
	}

	Vector3D secondIntersection;
	size_t secondIntersectionIndex = GetAllVertsUntilSecondIntersection
	(
		Normal,
		Center,
		ToClip,
		OutFace,
		firstIntersectionIndex,
		secondIntersection
	);

	if (!IsPointInPolygon(Normal, OutFace, Center))
	{
		GetFaceReveresed
		(
			IntersectFace,
			ToClip,
			OutFace,
			firstIntersectionIndex,
			firstIntersection,
			secondIntersection,
			secondIntersectionIndex
		);
	}
	IntersectFace.Vertices.Add(firstIntersection);
	IntersectFace.Vertices.Add(secondIntersection);

}

void PlaneClipping::GetFirstIntersection(const Vector3D& Normal, const Vector3D& Center, const Face& CurrentFace,
	Face& NewFace, size_t& FirstIntersectionIndex, Vector3D& FirstIntersection)
{
	for (FirstIntersectionIndex; FirstIntersectionIndex < CurrentFace.Vertices.GetSize(); FirstIntersectionIndex++)
	{
		Vector3D fromVert = CurrentFace.Vertices[FirstIntersectionIndex];
		Vector3D toVert = CurrentFace.Vertices[(FirstIntersectionIndex + 1) % CurrentFace.Vertices.GetSize()];

		Vector3D lineDir = toVert - fromVert;
		Vector3D intersectionPoint;
		const bool bDoesLineIntersect = Vector3D::GetIntersectionPointWithPlane(Center, Normal, fromVert, lineDir, intersectionPoint);

		if (bDoesLineIntersect)
		{
			const bool intersectionIsNextVertex = intersectionPoint == toVert;

			if (intersectionIsNextVertex)
			{
				NewFace.Vertices.Add(toVert);
				NewFace.Vertices.Add(CurrentFace.Vertices[(FirstIntersectionIndex + 2) % CurrentFace.Vertices.GetSize()]);
				FirstIntersectionIndex = (FirstIntersectionIndex + 2) % CurrentFace.Vertices.GetSize();

			}
			else
			{
				NewFace.Vertices.Add(intersectionPoint);
				NewFace.Vertices.Add(toVert);
				FirstIntersectionIndex = (FirstIntersectionIndex + 1) % CurrentFace.Vertices.GetSize();
			}

			FirstIntersection = intersectionPoint;

			break;

		}

	}
}

size_t PlaneClipping::GetAllVertsUntilSecondIntersection(const Vector3D& Normal, const Vector3D& Center,
	const Face& CurrentFace, Face& NewFace, const size_t& FirstIntersectionIndex, Vector3D& SecondIntersection)
{
	size_t secondIntersectionIndex = 0;

	for (secondIntersectionIndex = FirstIntersectionIndex; secondIntersectionIndex < CurrentFace.Vertices.GetSize(); secondIntersectionIndex++)
	{
		Vector3D fromVert = CurrentFace.Vertices[secondIntersectionIndex];
		Vector3D toVert = CurrentFace.Vertices[(secondIntersectionIndex + 1) % CurrentFace.Vertices.GetSize()];

		Vector3D lineDir = toVert - fromVert;
		Vector3D intersectionPoint;
		const bool bDoesLineIntersect = Vector3D::GetIntersectionPointWithPlane(Center, Normal, fromVert, lineDir, intersectionPoint);

		if (bDoesLineIntersect)
		{
			NewFace.Vertices.Add(intersectionPoint);
			SecondIntersection = intersectionPoint;
			secondIntersectionIndex++;
			break;

		}
		NewFace.Vertices.Add(toVert);
	}

	return secondIntersectionIndex;
}

bool PlaneClipping::IsPointInPolygon(const Vector3D& Normal, const Face& OutFace, const Vector3D& Center)
{
	//Check if any point is on the wrong side of the plane, i.e. on the negative side of the normal
	for (const auto& vert : OutFace.Vertices)
	{
		const double d = Vector3D::Dot(vert - Center, Normal);

		if (d < 0 && !MathCore::IsNearlyZero(d)) return false;
	}
	return true;

}

void PlaneClipping::GetFaceReveresed(Face& IntersectFace, const Face& CurrentFace, Face& NewFace, const size_t& FirstIntersectionIndex, const Vector3D&
	FirstIntersection, const Vector3D& SecondIntersection, const size_t& SecondIntersectionIndex)
{
	NewFace.Vertices.Empty();

	if (SecondIntersection != CurrentFace.Vertices[SecondIntersectionIndex % CurrentFace.Vertices.GetSize()]) NewFace.Vertices.Add(SecondIntersection);

	Vector3D from;

	for (size_t a = SecondIntersectionIndex; a % CurrentFace.Vertices.GetSize() != FirstIntersectionIndex; a++)
	{
		from = CurrentFace.Vertices[a % CurrentFace.Vertices.GetSize()];
		Vector3D to = CurrentFace.Vertices[(a + 1) % CurrentFace.Vertices.GetSize()];
		if (from == to) continue;

		NewFace.Vertices.Add(from);
	}

	if (from != FirstIntersection) NewFace.Vertices.Add(FirstIntersection);


}
