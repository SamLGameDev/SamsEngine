
#include "Voronoi2D.h"

#include <iostream>

#include "LinkedList.h"
#include "MathCore.h"

#include "Renderer.h"

#include "WireShapes.h";

Shader FracturePiece::PointShader;


//Possible floaing point error causing points to break

bool Voronoi2D::GetIntersection(float a, float b, float c,  Vector2D From, Vector2D To, Vector2D& intersection)
{
	float lineA = From.Y - To.Y;
	float lineB = To.X - From.X;
	float lineC = From.X * To.Y - To.X * From.Y;

	//if (fabs(b) < std::pow(2, -23) || fabs(lineB) < std::pow(2, -23))
	//	return false;

	//if (fabs(a * lineB - lineA * b) < std::pow(2, -23) &&
	//	fabs(c * lineB - lineC * b) < std::pow(2, -23))
	//	return false;

	if ((a / b == lineA / lineB) && (c / b == lineC / lineB)) return false;


	Vector3D u = Vector3D(a, b, c);
	Vector3D v = Vector3D(lineA, lineB, lineC);
	Vector3D z = Vector3D::Cross(u, v);

	if (z.Z == 0.f)
	{
		return false;
	}
	intersection = Vector2D(z.X / z.Z, z.Y / z.Z);

	bool IsVertical = MathCore::IsNearlyEqual(From.X, To.X, std::pow(2, -23));

	bool IsHorizontal = MathCore::IsNearlyEqual(From.Y, To.Y, std::pow(2, -23));

	bool IsEndpointY = MathCore::IsNearlyEqual(intersection.Y, From.Y, std::pow(2, -23)) || MathCore::IsNearlyEqual(intersection.Y, To.Y, std::pow(2, -23));
	bool IsEndpointX = MathCore::IsNearlyEqual(intersection.X, From.X, std::pow(2, -23)) || MathCore::IsNearlyEqual(intersection.X, To.X, std::pow(2, -23));

	bool IsBetweenXAxis = (intersection.X < From.X) != (intersection.X < To.X);
	bool IsBetweenYAxis = (intersection.Y < From.Y) != (intersection.Y < To.Y);

	bool IsBetweenAB = IsBetweenXAxis && IsBetweenYAxis;

	if (IsVertical && (IsEndpointY || IsBetweenYAxis)) return true;
	
	if (IsHorizontal && (IsEndpointX || IsBetweenXAxis)) return true;
	
	if (IsBetweenAB) return true;
	return false;
}

void Voronoi2D::FracturePlaneRandom(Vector2D TopLeft, Vector2D BottomLeft, Vector2D TopRight, Vector2D BottomRight)
{

	FracturePiece::PointShader = Shader("Point2D", "Shaders/");


	Vector2D center = BottomLeft + (TopRight / 2);

	Array<Vector2D> points;

	for (size_t i = 0; i < 10; i++)
	{
		Vector2D point1 = Vector2D::RandomRange(BottomLeft, TopRight);

		while (points.Contains(point1))
		{
			point1 = Vector2D::RandomRange(BottomLeft, TopRight);
		}

		points.Add(point1);
	}

	//points = { {0, 0},{0.2, -0.7}, Vector2D(-1, -1)};

	//points = { {0, 0} };



	for (size_t p = 0; p < points.GetSize(); p++)
	{
		Vector2D point = points[p];
		Array<Vector2D> cell = {TopLeft, TopRight, BottomRight, BottomLeft};

		for (size_t j = 0; j < points.GetSize(); j++)
		{
			if (j == p) continue;

			Vector2D qPoint = points[j];

			float a, b, c;
			Vector2D halfWay = Vector2D::PerpendicularBisector(point, qPoint, a, b, c);

			if (a == 0.f && b == 0.f) continue;

			Array<Vector2D> newCell;
			size_t intersections = 0;

			size_t FirstIntersectionIndex;

			size_t SecondIntersectionIndex = 0;

			Vector2D firstIntersection;

			for (size_t i = 0; i < cell.GetSize(); i++)
			{
				Vector2D from = cell[i];
				Vector2D to = cell[(i + 1) % cell.GetSize()];

				bool IsIntersection = GetIntersection(a, b, c, from, to, firstIntersection);

				if (IsIntersection)
				{
					bool intersectionIsNextVertex = firstIntersection == to;

					if (intersectionIsNextVertex)
					{
						newCell.Add(to);
						newCell.Add(cell[(i + 2) % cell.GetSize()]);
						FirstIntersectionIndex = (i + 2) % cell.GetSize();

					}
					else
					{
						newCell.Add(firstIntersection);
						newCell.Add(to);
						FirstIntersectionIndex = (i + 1) % cell.GetSize();
					}

					break;

				}

			}

			if (newCell.GetSize() == 0)
			{
				newCell = cell;
			}
			else
			{
				Vector2D secondIntersection;

				for (size_t i = FirstIntersectionIndex; i < cell.GetSize(); i++)
				{
					//size_t idx = (FirstIntersectionIndex + i) % cell.GetSize();
					size_t idx = i;

					Vector2D from = cell[idx];
					Vector2D to = cell[(idx + 1) % cell.GetSize()];

					Vector2D intersection;

					bool IsIntersection = GetIntersection(a, b, c, from, to, intersection);

					if (IsIntersection)
					{
						newCell.Add(intersection);
						SecondIntersectionIndex = idx + 1;
						secondIntersection = intersection;
						break;
					}
					else
					{
						newCell.Add(to);
					}

				}
				if (!IsPointInPolygon(point, newCell))
				{
					newCell.Empty();

					if (secondIntersection != cell[SecondIntersectionIndex % cell.GetSize()]) newCell.Add(secondIntersection);

					Vector2D from;

					for (size_t i = SecondIntersectionIndex; i % cell.GetSize() != FirstIntersectionIndex; i++)
					{
						from = cell[i % cell.GetSize()];
						Vector2D to = cell[(i + 1) % cell.GetSize()];
						if (from == to) continue;

						newCell.Add(from);
					}

					if (from != firstIntersection) newCell.Add(firstIntersection);
				}

			}

			cell = newCell;
		}

		FracturePiece* frac = new FracturePiece(cell, point);
	}

}

bool Voronoi2D::IsPointInPolygon(Vector2D Point, Array<Vector2D> Polygon)
{
	for (size_t i = 0; i < Polygon.GetSize(); i++)
	{
		Vector2D t = { Polygon[i].X - Polygon[(i + 1) % Polygon.GetSize()].X,   Polygon[i].Y - Polygon[(i + 1) % Polygon.GetSize()].Y };
		Vector2D u = {Point.X - Polygon[(i + 1) % Polygon.GetSize()].X, Point.Y - Polygon[(i + 1) % Polygon.GetSize()].Y };
		Vector2D v = {Polygon[(i+2)%Polygon.GetSize()].X - Polygon[(i + 1) % Polygon.GetSize()].X  , Polygon[(i + 2) % Polygon.GetSize()].Y - Polygon[(i + 1) % Polygon.GetSize()].Y };

		if (!(Vector2D::Cross(t, u) * Vector2D::Cross(t, v) >= 0 && Vector2D::Cross(v, u) * Vector2D::Cross(v, t) >= 0)) return false;

	}

	return true;
}
FracturePiece::FracturePiece(Array<Vector2D> cell, Vector2D Point)
{

//	cell = { {0, 1},  { 1,1 }, {1, -1}, {0, -1}, {0, -1}, {-1, -1}, {-1, 1}, {0, 1} };
	//cell = {{0.5, 0.5}, {1, 1}, {-1 , -1}};

	//Array<Vector2D> solo;

	//for (Vector2D vert : cell)
	//{
	//	unsigned int index;
	//	if (solo.Contains(vert, index))
	//	{
	//		continue;
	//	}
	//	solo.Add(vert);
	//}

	//cell = solo;

	for (Vector2D vert : cell)
	{
		Verts.Add(vert.X);
		Verts.Add(vert.Y);
	}

	//std::cout << "Inds: ";
	//for (size_t i = 0; i < cell.GetSize() || i % 3 != 0; i++)
	//{
	//	Inds.Add(i % cell.GetSize());
	//	std::cout << i % cell.GetSize();
	//}

	for (size_t i = 1; i + 1 < cell.GetSize(); i++)
	{
		Inds.Add(0);
		Inds.Add(i);
		Inds.Add(i + 1);
	}

	//Inds = { 0, 1, 3,  // first Triangle
	//	1, 2, 3 };

	//Verts = { 0.5f,  0.5f, 0.0f,  // top right
	//	 0.5f, -0.5f, 0.0f,  // bottom right
	//	-0.5f, -0.5f, 0.0f,  // bottom left
	//	-0.5f,  0.5f, 0.0f };

	//std::cout << "Vert: " << "\n";

	//for (Vector2D vert : Verts)
	//{

	//	vert.Print();
	//}

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	glBufferData(GL_ARRAY_BUFFER, Verts.GetSize() * sizeof(float), Verts.GetFirstRef(), GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

	glBufferData(GL_ELEMENT_ARRAY_BUFFER, Inds.GetSize() * sizeof(int), Inds.GetFirstRef(), GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);

	glBindVertexArray(0);

	glGenVertexArrays(1, &PVAO);
	glGenBuffers(1, &PVBO);

	glBindVertexArray(PVAO);

	glBindBuffer(GL_ARRAY_BUFFER, PVBO);

	Array<float> Points = { Point.X, Point.Y };

	glBufferData(GL_ARRAY_BUFFER, 2 * sizeof(float), Points.GetFirstRef(), GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);


	color = Vector3D::RandomRange(Vector3D(30, 30, 30), Vector3D(255, 255, 255));


	Renderer::FracturesToDraw.Add(this);
}

void FracturePiece::Draw(const Shader* InShader)
{
	InShader->Use();
	InShader->SetVec3("Color", color);

	//std::cout << "DrawCalled";

	//TODO find a way to separate this from model
	//used for reflection InShade
	glPointSize(5);
	glBindVertexArray(VAO);
	//glBindBuffer(GL_ARRAY_BUFFER, VBO);
	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(Inds.GetSize()), GL_UNSIGNED_INT, nullptr);
	glBindVertexArray(0);
	glUseProgram(0);

	PointShader.Use();
	glBindVertexArray(PVAO);
	//glBindBuffer(GL_ARRAY_BUFFER, VBO);
	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glDrawArrays(GL_POINTS, 0, 1);
	glBindVertexArray(0);
	glUseProgram(0);
}

