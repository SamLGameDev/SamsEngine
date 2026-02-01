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

		Vector3D normal = Vector3D::Cross(bound.Vertices[1].point - bound.Vertices[0].point, bound.Vertices[2].point - bound.Vertices[0].point).Normalised();

		if (Vector3D::Dot(normal, bound.Vertices[0].point - Center) < 0) normal = -normal;

		const double d = -Vector3D::Dot(normal, bound.Vertices[0].point);
		for (size_t i = 0; i < OutClipped.Vertices.GetSize(); i++)
		{
			const Vector3D& point = OutClipped.Vertices[i];
			const Vector3D& next = OutClipped.Vertices[(i + 1) % OutClipped.Vertices.GetSize()];
			const double d1 = Vector3D::Dot(normal, point) + d;
			const double d2 = Vector3D::Dot(normal, next) + d;

			const auto inside = [](const double& delta) { return delta <= 0; };
			const auto outside = [](const double& delta) { return delta > 0; };
			const auto onPlane = [](const double& delta) { return MathCore::IsNearlyZero(delta, 0.001); };

			if (inside(d1) && inside(d2) || (outside(d1) && onPlane(d2))) newFace.Vertices.Add(next);

			else if (MathCore::IsNearlyZero(d1, 0.001) && d2 > 0) continue;

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

		if (newFace.Vertices.GetSize() < 3)
		{
			OutClipped.Vertices.Empty();
			return;
		}

		OutClipped = newFace;
	}

	//Vector3D normal = Vector3D::Cross(OutClipped.Vertices[1] - OutClipped.Vertices[0], OutClipped.Vertices[2] - OutClipped.Vertices[0]).Normalised();

	//if (Vector3D::Dot(normal, OutClipped.Vertices[0] - Center) < 0) normal = -normal;

	//VoronoiFace orderedFace;

	//Voronoi::OrderVertices(OutClipped.Vertices, Center, normal, orderedFace);

	//OutClipped.Vertices.Empty();

	//for (const auto& vert : orderedFace.Vertices)
	//{
	//	OutClipped.Vertices.Add(vert.point);
	//}

}

void SutherlandHodgeman::Clip3D(const Array<FTriangle>& Bounds, const FTriangle& ToClip, Face& OutClipped,
	const Vector3D& Center)
{
	OutClipped.Vertices = { ToClip.Verts[0], ToClip.Verts[1], ToClip.Verts[2] };


	for (const auto& bound : Bounds)
	{
		Face newFace;

		Vector3D normal = Vector3D::Cross(bound[1] - bound[0], bound[2] - bound[0]).Normalised();

		if (Vector3D::Dot(normal, bound[0] - Center) < 0) normal = -normal;

		const double d = -Vector3D::Dot(normal, bound[0]);
		for (size_t i = 0; i < OutClipped.Vertices.GetSize(); i++)
		{
			const Vector3D& point = OutClipped.Vertices[i];
			const Vector3D& next = OutClipped.Vertices[(i + 1) % OutClipped.Vertices.GetSize()];
			const double d1 = Vector3D::Dot(normal, point) + d;
			const double d2 = Vector3D::Dot(normal, next) + d;

			const auto inside = [](const double& delta) { return delta <= 0; };
			const auto outside = [](const double& delta) { return delta > 0; };
			const auto onPlane = [](const double& delta) { return MathCore::IsNearlyZero(delta, 0.001); };

			if (inside(d1) && inside(d2) || (outside(d1) && onPlane(d2))) newFace.Vertices.Add(next);

			else if (MathCore::IsNearlyZero(d1, 0.001) && d2 > 0) continue;

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

		if (newFace.Vertices.GetSize() < 3)
		{
			OutClipped.Vertices.Empty();
			return;
		}

		Vector3D n = Vector3D::Cross(OutClipped.Vertices[1] - OutClipped.Vertices[0], OutClipped.Vertices[2] - OutClipped.Vertices[0]).Normalised();

		if (Vector3D::Dot(normal, OutClipped.Vertices[0] - Center) < 0) n = -n;

		VoronoiFace orderedFace;

		Voronoi::OrderVertices(OutClipped.Vertices, Center, normal, orderedFace);

		OutClipped.Vertices.Empty();

		for (const auto& vert : orderedFace.Vertices)
		{
			OutClipped.Vertices.Add(vert.point);
		}

		OutClipped = newFace;
	}

}

void SutherlandHodgeman::Clip3D(const Array<FTriangle>& Bounds, const Array<VoronoiFace>& ToClip, Array<Face>& OutClipped,
                                const Vector3D& Center)
{

	//OutClipped.Vertices = { ToClip.Verts[0], ToClip.Verts[1], ToClip.Verts[2] };

	for (auto& face : ToClip)
	{
		Face newFace;
		for (const auto& vert : face.Vertices)
		{
			newFace.Vertices.Add(vert.point);
		}

		if (newFace.Vertices.GetSize() < 3) continue;

		OutClipped.Add(newFace);
	}

	for (const auto& bound : Bounds)
	{
		Array<Face> newFaces;
		for (const auto& vface : OutClipped)
		{
			Face newFace;

			Vector3D normal = Vector3D::Cross(bound[1] - bound[0], bound[2] - bound[0]).Normalised();

			if (Vector3D::Dot(normal, bound[0] - Center) < 0) normal = -normal;

			const double d = -Vector3D::Dot(normal, bound[0]);
			for (size_t i = 0; i < vface.Vertices.GetSize(); i++)
			{
				const Vector3D& point = vface.Vertices[i];
				const Vector3D& next = vface.Vertices[(i + 1) % vface.Vertices.GetSize()];
				const double d1 = Vector3D::Dot(normal, point) + d;
				const double d2 = Vector3D::Dot(normal, next) + d;

				const auto inside = [&](const double& d) { return d <= 0; };
				const auto outside = [&](const double& d) { return d > 0; };
				const auto onPlane = [&](const double& d) { return MathCore::IsNearlyZero(d); };

				if (inside(d1) && inside(d2) || (outside(d1) && onPlane(d2))) newFace.Vertices.Add(next);

				else if (MathCore::IsNearlyZero(d1) && d2 > 0) continue;

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
			newFaces.Add(newFace);

		}
		OutClipped = newFaces;
	}
}
