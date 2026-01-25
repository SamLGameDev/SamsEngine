#include <iostream>
#include <ostream>

#include "AABB.h"
#include "MathCore.h"
#include "SutherlandHodegman.h"
#include "Verticie.h"
#include "VoronoiClipping.h"

void SutherlandHodgeman::Clip3D(const Array<VoronoiFace>& Bounds, const FTriangle& ToClip, Face& OutClipped, const Vector3D& Center)
{

	OutClipped.Vertices = { ToClip.Verts[0], ToClip.Verts[1], ToClip.Verts[2] };


	for (const auto& bound : Bounds)
	{
		Face newFace;
		if (bound.Vertices.GetSize() < 3) continue;

		if (OutClipped.Vertices.GetSize() > 20)
		{
			std::cout << "Clipping failed, too many vertices" << std::endl;
		};

		Vector3D normal = Vector3D::Cross(bound.Vertices[1].point - bound.Vertices[0].point, bound.Vertices[2].point - bound.Vertices[0].point).Normalised();

		if (Vector3D::Dot(normal, bound.Vertices[0].point - Center) < 0) normal = -normal;

		const double d = -Vector3D::Dot(normal, bound.Vertices[0].point);
		for (size_t i = 0; i < OutClipped.Vertices.GetSize(); i++)
		{
			const Vector3D& point = OutClipped.Vertices[i];
			const Vector3D& next = OutClipped.Vertices[(i + 1) % OutClipped.Vertices.GetSize()];
			const double d1 = Vector3D::Dot(normal, point) + d;
			const double d2 = Vector3D::Dot(normal, next) + d;

			const auto inside = [&](const double& d) { return d <= 0; };
			const auto outside = [&](const double& d) { return d > 0; };
			const auto onPlane = [&](const double& d) { return MathCore::IsNearlyZero(d); };

			if (inside(d1) && inside(d2) || (outside(d1) && onPlane(d2))) newFace.Vertices.Add(next);

			else if (MathCore::IsNearlyZero(d1) && d2 > 0) return;

			else if (inside(d1) && outside(d2))
			{
				const Vector3D intersectPoint = Vector3D::GetLineIntersectionPointWithPlane(normal, d, point, next);
				newFace.Vertices.Add(intersectPoint);
			}

			else if (outside(d1) && inside(d2))
			{
				const Vector3D intersectPoint = Vector3D::GetLineIntersectionPointWithPlane(normal, d, point, next);

				newFace.Vertices.Add(intersectPoint);
				newFace.Vertices.Add(next);
			}

		}
		OutClipped = newFace;
	}
}
