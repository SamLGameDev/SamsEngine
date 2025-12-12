
#include "Voronoi2D.h"

#include <iostream>

#include "DataBuffers.h"
#include "LinkedList.h"
#include "MathCore.h"

#include "Renderer.h"

#include "WireShapes.h";

Shader FracturePiece2D::PointShader;


bool Voronoi2D::GetIntersection(const float& A, const float& B, const float& c, const Vector2D& From, const Vector2D& To, Vector2D& Intersection)
{
	const float lineA = From.Y - To.Y;
	const float lineB = To.X - From.X;
	const float lineC = From.X * To.Y - To.X * From.Y;

	if (MathCore::IsNearlyEqual(A / B, lineA / lineB) 
		&& MathCore::IsNearlyEqual(c / B, lineC / lineB)) return false;


	const Vector3D u = Vector3D(A, B, c);
	const Vector3D v = Vector3D(lineA, lineB, lineC);
	const Vector3D z = Vector3D::Cross(u, v);

	if (z.Z == 0.f) return false;
	
	Intersection = Vector2D(z.X / z.Z, z.Y / z.Z);

	const bool IsVertical = MathCore::IsNearlyEqual(From.X, To.X, std::pow(2, -23));

	const bool IsHorizontal = MathCore::IsNearlyEqual(From.Y, To.Y, std::pow(2, -23));

	const bool IsEndpointY = MathCore::IsNearlyEqual(Intersection.Y, From.Y, std::pow(2, -23))
	|| MathCore::IsNearlyEqual(Intersection.Y, To.Y, std::pow(2, -23));
	const bool IsEndpointX = MathCore::IsNearlyEqual(Intersection.X, From.X, std::pow(2, -23)) 
	|| MathCore::IsNearlyEqual(Intersection.X, To.X, std::pow(2, -23));

	const bool IsBetweenXAxis = Intersection.X < From.X != Intersection.X < To.X;
	const bool IsBetweenYAxis = Intersection.Y < From.Y != Intersection.Y < To.Y;

	const bool IsBetweenAB = IsBetweenXAxis && IsBetweenYAxis;

	if (IsVertical && (IsEndpointY || IsBetweenYAxis)) return true;
	
	if (IsHorizontal && (IsEndpointX || IsBetweenXAxis)) return true;
	
	if (IsBetweenAB) return true;

	return false;
}

void Voronoi2D::GetFirstIntersection(const Array<Vector2D>& Cell, const float& A, const float& B, const float& C, Array<Vector2D>& NewCell, size_t& FirstIntersectionIndex, Vector2D
                                     & FirstIntersection)
{
	for (size_t i = 0; i < Cell.GetSize(); i++)
	{
		const Vector2D& from = Cell[i];
		const Vector2D& to = Cell[(i + 1) % Cell.GetSize()];

		const bool bIsIntersection = GetIntersection(A, B, C, from, to, FirstIntersection);

		if (bIsIntersection)
		{
			const bool bIntersectionIsNextVertex = FirstIntersection == to;

			if (bIntersectionIsNextVertex)
			{
				NewCell.Add(to);
				NewCell.Add(Cell[(i + 2) % Cell.GetSize()]);
				FirstIntersectionIndex = (i + 2) % Cell.GetSize();
				return;
			}
			
			NewCell.Add(FirstIntersection);
			NewCell.Add(to);
			FirstIntersectionIndex = (i + 1) % Cell.GetSize();
			return;
		}

	}
}

void Voronoi2D::GetSecondIntersection(const Array<Vector2D>& Cell, const float& A, const float& B, const float& C, Array<Vector2D>& NewCell, const size_t&
                                      FirstIntersectionIndex, size_t& SecondIntersectionIndex, Vector2D& SecondIntersection)
{
	for (size_t i = FirstIntersectionIndex; i < Cell.GetSize(); i++)
	{
		const Vector2D& from = Cell[i];
		const Vector2D& to = Cell[(i + 1) % Cell.GetSize()];

		Vector2D intersection;

		const bool bIsIntersection = GetIntersection(A, B, C, from, to, intersection);

		if (bIsIntersection)
		{
			NewCell.Add(intersection);
			SecondIntersectionIndex = i + 1;
			SecondIntersection = intersection;
			return;
		}
		
		NewCell.Add(to);
		

	}
}

void Voronoi2D::GetReversePolygon(const Array<Vector2D>& Cell, Array<Vector2D>& NewCell, const size_t& FirstIntersectionIndex, const size_t&
                                  SecondIntersectionIndex, const Vector2D& FirstIntersection, const Vector2D& SecondIntersection)
{
	NewCell.Empty();

	if (SecondIntersection != Cell[SecondIntersectionIndex % Cell.GetSize()]) NewCell.Add(SecondIntersection);

	Vector2D from;

	for (size_t i = SecondIntersectionIndex; i % Cell.GetSize() != FirstIntersectionIndex; i++)
	{
		from = Cell[i % Cell.GetSize()];
		const Vector2D& to = Cell[(i + 1) % Cell.GetSize()];
		if (from == to) continue;

		NewCell.Add(from);
	}

	if (from != FirstIntersection) NewCell.Add(FirstIntersection);
}

void Voronoi2D::FracturePlaneRandom(const Vector2D& TopLeft, const Vector2D& BottomLeft, const Vector2D& TopRight, const Vector2D& BottomRight, const
                                    size_t& NumPoints)
{

	const Vector2D center = BottomLeft + (TopRight / 2);

	Array<Vector2D> points(NumPoints);

	for (size_t i = 0; i < NumPoints; i++)
	{
		Vector2D point1;
		do
		{
			point1 = Vector2D::RandomRange(BottomLeft, TopRight);
		} while (points.Contains(point1));

		points[i] = point1;
	}

	//points = { {0, 0},{0.2, -0.7}, Vector2D(-1, -1)};

	//points = { {0, 0} };

	Fractures.Reallocate(NumPoints);

	for (size_t p = 0; p < points.GetSize(); p++)
	{
		const Vector2D& point = points[p];
		Array<Vector2D> cell = {TopLeft, TopRight, BottomRight, BottomLeft};
	
		for (size_t j = 0; j < points.GetSize(); j++)
		{
			if (j == p) continue;

			const Vector2D& qPoint = points[j];

			float a, b, c;
			const Vector2D& halfWay = Vector2D::PerpendicularBisector(point, qPoint, a, b, c);
			
			//No valid bi-sector as p and q are perpendicular
			if (a == 0.f && b == 0.f) continue;

			Array<Vector2D> newCell;

			size_t firstIntersectionIndex;

			size_t secondIntersectionIndex = 0;

			Vector2D firstIntersection;

			GetFirstIntersection(cell, a, b, c, newCell, firstIntersectionIndex, firstIntersection);

			//If no intersection, keep original cell
			if (newCell.IsEmpty())
			{
				continue;
			}

			//Add all cells up to and including the second intersection
			Vector2D secondIntersection;

			GetSecondIntersection(cell, a, b, c, newCell, firstIntersectionIndex, secondIntersectionIndex, secondIntersection);

			//There are 2 cases that can happen here, either the point is inside the new polygon, or its outside
			//If outside, get the polygon from second intersection to first, as that will contain the point.
			if (!IsPointInPolygon(point, newCell))
			{
				GetReversePolygon(cell, newCell, firstIntersectionIndex, secondIntersectionIndex, firstIntersection, secondIntersection);
			}

			cell = newCell;
		}

		Fractures[p] = FracturePiece2D(cell, point);
	}

}

bool Voronoi2D::IsPointInPolygon(const Vector2D& Point, const Array<Vector2D>& Polygon)
{
	for (size_t i = 0; i < Polygon.GetSize(); i++)
	{
		//checks if the point is inside the shape formed by polygon->polygon(i+1)
		// and polygon(i+1)->Polygon(i+2)

		const Vector2D t = Polygon[i] - Polygon[(i + 1) % Polygon.GetSize()];
		const Vector2D u = Point - Polygon[(i + 1) % Polygon.GetSize()];
		const Vector2D v = Polygon[(i+2)%Polygon.GetSize()] - Polygon[(i + 1) % Polygon.GetSize()];

		if (!(Vector2D::Cross(t, u) * Vector2D::Cross(t, v) >= 0 && Vector2D::Cross(v, u) * Vector2D::Cross(v, t) >= 0)) return false;

	}

	return true;
}
FracturePiece2D::FracturePiece2D(const Array<Vector2D>& cell, const Vector2D& Point)
{
	shader = Shader("ColorShape", "/Shaders/");

	for (Vector2D vert : cell)
	{
		Verts.Add(vert.X);
		Verts.Add(vert.Y);
	}

	for (size_t i = 1; i + 1 < cell.GetSize(); i++)
	{
		Inds.Add(0);
		Inds.Add(i);
		Inds.Add(i + 1);
	}

	::DataBuffers::GenBuffer(VAO);


	DataBuffers::BindVertexInfo(VAO, 0, 0, sizeof(Vector2D), 0, Vector2);


	::DataBuffers::BufferData(VAO, Verts.GetSize() * sizeof(float), Verts.GetFirstPtr(), BufferTargets::VERTEX);
	DataBuffers::BufferDataIndex(VAO, Inds.GetSize() * sizeof(uint16_t), Inds.GetFirstPtr());

	::Renderer::AddFracture(this);

	color = Vector3D::RandomRange(Vector3D(30, 30, 30), Vector3D(255, 255, 255));

	transform.Position = { 0, 0, -3 };
}

void FracturePiece2D::Draw()
{
	shader.Use();

	DataBuffers::BindBuffer(VAO);

	DataBuffers::DrawVertexData(VAO);

	PerInstanceTransforms ubo;
	ubo.Model = transform.GetModelMatrix();
	ubo.Color = color;

	GlobalTransforms g;
	g.View = Camera::GetActiveCamera()->GetLook();

	g.Projection = Camera::GetActiveCamera()->GetProjection();

	shader.SetUniformBuffer(0, &g, sizeof(GlobalTransforms));

	shader.SetUniformBuffer(1, &ubo, sizeof(PerInstanceTransforms));

	::Renderer::Draw(Inds.GetSize());
}
