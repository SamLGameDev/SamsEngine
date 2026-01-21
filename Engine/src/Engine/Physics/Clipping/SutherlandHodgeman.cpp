#include "AABB.h"
#include "MathCore.h"
#include "SutherlandHodegman.h"
#include "Verticie.h"
#include "VoronoiClipping.h"

void SutherlandHodgeman::Clip3D(const Array<VoronoiFace>& Bounds, const FTriangle& ToClip, Face& OutClipped, const Vector3D& Center)
{

	OutClipped.Vertices = { ToClip.Verts[0], ToClip.Verts[1], ToClip.Verts[2] };
	Face newFace;

	for (const auto& bound : Bounds)
	{
		Vector3D normal;
		for (size_t i = 0; i < bound.Vertices.GetSize(); i++)
		{
			const Vector3D& current = bound.Vertices[i].point;
			const Vector3D& next = bound.Vertices[(i + 1) % bound.Vertices.GetSize()].point;
			normal.X += (current.Y - next.Y) * (current.Z + next.Z);
			normal.Y += (current.Z - next.Z) * (current.X + next.X);
			normal.Z += (current.X - next.X) * (current.Y + next.Y);
		}
		normal = normal.Normalised();

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
		//face = newFace;
	}
}
