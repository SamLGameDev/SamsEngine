#include "DelaunayTriangulation.h"
#include "Camera.h"

void DelaunayTriangulation::Triangulate(Array<Vector3D>& Vertices, Array<size_t>& Indicies)
{
	Triangle superTriangle = GetSuperTriangle(Vertices);

	Array<Triangle> Triangles = { superTriangle };

	for (const auto& point : Vertices)
	{
		for (const auto& triangle : Triangles)
		{
			//Is Point inside triangle?
			//Connect to triangle
			//Remove Super triangle rsulting triangles?? Hwow
			//if triangle contains super triangle point?
			/// too inefficnet?
			///Seperate triangulation from Indexing? 
			/// Use the old method? Maps? Better method?}
			/// //Each triangle point has an associated index? tah twe assign
		}
	}


}

Triangle DelaunayTriangulation::GetSuperTriangle(const Array<Vector3D>& Vertices)
{
	Vector2D XRange = { std::numeric_limits<float>::max(), std::numeric_limits<float>::min() };
	Vector2D YRange = { std::numeric_limits<float>::max(), std::numeric_limits<float>::min() };
	Vector2D ZRange = { std::numeric_limits<float>::max(), std::numeric_limits<float>::min() };

	for(const auto& vertex : Vertices)
	{
		if (vertex.X < XRange.X)
		{
			XRange.X = vertex.X;
		}
		if (vertex.X > XRange.Y)
		{
			XRange.Y = vertex.X;
		}
		if (vertex.Y < YRange.X)
		{
			YRange.X = vertex.Y;
		}
		if (vertex.Y > YRange.Y)
		{
			YRange.Y = vertex.Y;
		}
		if (vertex.Z < ZRange.X)
		{
			ZRange.X = vertex.Z;
		}
		if (vertex.Z > ZRange.Y)
		{
			ZRange.Y = vertex.Z;
		}
	}

	const float diffX = XRange.Y - XRange.X;
	const float diffZ = ZRange.Y - ZRange.X;


	return { {XRange.X, YRange.X, ZRange.X}, {XRange.X + (diffX / 2), YRange.Y, ZRange.X + (diffZ /2)}, {XRange.Y, YRange.X, ZRange.Y}};
}
