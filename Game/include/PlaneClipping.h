


#pragma once
#include "Array.h"
#include "Verticie.h"

class PlaneClipping
{
public:

	static void ClipCellByFaces(Array<Face>& ToClip, const Array<Face>& ClippingPlanes);
	static void ClipCellByFace(Array<Face>& ToClip, const Vector3D& Center, const Vector3D& normal);
	static void ClipFaceByFace(const Face& ToClip, const Vector3D& Center, Face& OutFace, const Vector3D& Normal, Face& IntersectFace);

private:
	static void GetFirstIntersection(const Vector3D& Normal, const Vector3D& Center, const Face& CurrentFace, Face& NewFace, size_t& FirstIntersectionIndex, Vector3D& FirstIntersection);
	static size_t GetAllVertsUntilSecondIntersection(const Vector3D& Normal, const Vector3D& Center, const Face& CurrentFace, Face& NewFace,
		const size_t& FirstIntersectionIndex, Vector3D& SecondIntersection);
	static bool IsPointInPolygon(const Vector3D& Normal, const Face& OutFace, const Vector3D& Center);
	static void GetFaceReveresed(Face& IntersectFace, const Face& CurrentFace, Face& NewFace,
	                      const size_t& FirstIntersectionIndex,
	                      const Vector3D& FirstIntersection, const Vector3D& SecondIntersection,
	                      const size_t& SecondIntersectionIndex);
};
