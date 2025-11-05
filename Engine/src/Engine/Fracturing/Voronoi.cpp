
#include "Voronoi.h"
#include "transform.h"
#include <iostream>

#include "MathCore.h"
#include "Renderer.h"

void Voronoi::FracturePlaneRandom(Model& InModel)
{
	Vector3D center = InModel.BoundingBox->WireTransform.TransCenter;

	Array<Vector3D> points;



	//FracturePiece3D::PointShader = Shader("Point3D", "Shaders/");

	for (size_t i = 0; i < 100; i++)
	{
		Vector3D point1 = InModel.ModelTransform.GetRandomPointInBounds();

		while (points.Contains(point1))
		{
			point1 = InModel.ModelTransform.GetRandomPointInBounds();
		}
		TestSquare.push_back(DrawWireCube(point1, { 0.5, 0.5, 0.5 }, { 0.1f, 0.1f, 0.1f }, { 0.5, 0.5, 0.5 }));

		points.Add(point1);
	}

	//points = { {0.1, 0.1, 0.1}, {0.8, 0.4, 0.2}, {0.4, 0.8, 0.6}, {0.6, 0.2, 0.1} };
	//points = { {0.1, 0.1, 0.1}, {0.8, 0.4, 0.2} };
	for (Vector3D& point : points)
	{
		auto color = Vector3D::RandomRange(Vector3D(30, 30, 30), Vector3D(255, 255, 255));
		TestSquare.push_back(DrawWireCube(point, { 0.5, 0.5, 0.5 }, { 0.1f, 0.1f, 0.1f }, color / 255));
	}


	for (size_t i = 0; i < points.GetSize(); i++)
	{
		Vector3D currentPoint = points[i];

		Array<Face> Faces = InModel.BoundingBox->Faces;

		for (size_t j = 0; j < points.GetSize(); j++)
		{
			if (j == i)continue;

			Array<Face> newFaces;
			Vector3D comparedPoint = points[j];

			Vector3D normal, right, up, center;

			Face intersectFace;

			DefinePlane(normal, currentPoint, comparedPoint, right, up, center);

			//TestSquare.push_back(DrawWirePlane(center, normal, { 5, 5, 5 }, { 1, 1, 1 }));

			for (size_t f = 0; f < Faces.GetSize(); f++)
			{
				Face currentFace = Faces[f];
				Face newFace;

				size_t firstIntersectionIndex = 0;
				Vector3D firstIntersection;

				for (firstIntersectionIndex; firstIntersectionIndex < currentFace.Vertices.GetSize(); firstIntersectionIndex++)
				{
					Vector3D fromVert = currentFace.Vertices[firstIntersectionIndex];
					Vector3D toVert = currentFace.Vertices[(firstIntersectionIndex + 1) % currentFace.Vertices.GetSize()];

					Vector3D lineDir = toVert - fromVert;
					Vector3D intersectionPoint;
					const bool bDoesLineIntersect = Vector3D::GetIntersectionPointWithPlane(center, normal, fromVert, lineDir, intersectionPoint);

					if (bDoesLineIntersect)
					{
						const bool intersectionIsNextVertex = intersectionPoint == toVert;

						if (intersectionIsNextVertex)
						{
							newFace.Vertices.Add(toVert);
							newFace.Vertices.Add(currentFace.Vertices[(firstIntersectionIndex + 2) % currentFace.Vertices.GetSize()]);
							firstIntersectionIndex = (firstIntersectionIndex + 2) % currentFace.Vertices.GetSize();

						}
						else
						{
							newFace.Vertices.Add(intersectionPoint);
							newFace.Vertices.Add(toVert);
							firstIntersectionIndex = (firstIntersectionIndex + 1) % currentFace.Vertices.GetSize();
						}

						firstIntersection = intersectionPoint;

						break;
						
					}

				}
				if (newFace.Vertices.IsEmpty())
				{
					if (Vector3D::Dot(normal, (currentFace.Vertices[0] - center).Normalised()) > 0)
					{
						newFaces.Add(currentFace);
					}
					continue;
				}
				else
				{

					size_t secondIntersectionIndex = 0;
					Vector3D secondIntersection;

					for (secondIntersectionIndex = firstIntersectionIndex; secondIntersectionIndex < currentFace.Vertices.GetSize(); secondIntersectionIndex++)
					{
						Vector3D fromVert = currentFace.Vertices[secondIntersectionIndex];
						Vector3D toVert = currentFace.Vertices[(secondIntersectionIndex + 1) % currentFace.Vertices.GetSize()];

						Vector3D lineDir = toVert - fromVert;
						Vector3D intersectionPoint;
						const bool bDoesLineIntersect = Vector3D::GetIntersectionPointWithPlane(center, normal, fromVert, lineDir, intersectionPoint);

						if (bDoesLineIntersect)
						{
							newFace.Vertices.Add(intersectionPoint);
							secondIntersection = intersectionPoint;
							secondIntersectionIndex++;
							break;

						}
						newFace.Vertices.Add(toVert);
					}

					if (!IsPointInPolygon(normal, newFace.Vertices, center))
					{
						newFace.Vertices.Empty();

						if (secondIntersection != currentFace.Vertices[secondIntersectionIndex % currentFace.Vertices.GetSize()]) newFace.Vertices.Add(secondIntersection);

						Vector3D from;

						for (size_t a = secondIntersectionIndex; a % currentFace.Vertices.GetSize() != firstIntersectionIndex; a++)
						{
							from = currentFace.Vertices[a % currentFace.Vertices.GetSize()];
							Vector3D to = currentFace.Vertices[(a + 1) % currentFace.Vertices.GetSize()];
							if (from == to) continue;

							newFace.Vertices.Add(from);
						}

						if (from != firstIntersection) newFace.Vertices.Add(firstIntersection);

						bool bAddFirst = true, bAddSecond = true;
						for (auto vert : intersectFace.Vertices)
						{
							if (Vector3D::IsAlmostEqual(vert, firstIntersection)) bAddFirst = false;

							if (Vector3D::IsAlmostEqual(vert, secondIntersection)) bAddSecond = false;
						}

						if (bAddSecond) intersectFace.Vertices.Add(secondIntersection);
						if (bAddFirst) intersectFace.Vertices.Add(firstIntersection);
					}
					else
					{

						bool bAddFirst = true, bAddSecond = true;
						for (auto vert : intersectFace.Vertices)
						{
							if (Vector3D::IsAlmostEqual(vert, firstIntersection)) bAddFirst = false;

							if (Vector3D::IsAlmostEqual(vert, secondIntersection)) bAddSecond = false;
						}

						if (bAddFirst) intersectFace.Vertices.Add(firstIntersection);

						if (bAddSecond) intersectFace.Vertices.Add(secondIntersection);
					}
				}
				newFaces.Add(newFace);
			}
			newFaces.Add(intersectFace);

			Faces = newFaces;
		}
		auto color = Vector3D::RandomRange(Vector3D(30, 30, 30), Vector3D(255, 255, 255));
		for (auto& face : Faces)
		{
		//Face face = Faces[5];
			if (fractureFaces.Contains(face))
			{
				continue;
			}

			FracturePiece3D* frac = new FracturePiece3D(face.Vertices, currentPoint);
			frac->color = color;
			fractureFaces.Add(face);
		}
		//TestSquare.push_back(DrawWireCube(currentPoint, { 0.5, 0.5, 0.5 }, { 0.1f, 0.1f, 0.1f }, color/255));

	}

	
}
void Voronoi::DefinePlane(Vector3D& normal, Vector3D& CurrentPoint, Vector3D& closestPoint, Vector3D& Right, Vector3D& Up, Vector3D& PlaneCenter)
{
	normal = (CurrentPoint - closestPoint).Normalised();

	Vector3D arbitraryUp = Vector3D::Up;
	if (fabs(Vector3D::Dot(normal, arbitraryUp)) > 0.99f) {
		arbitraryUp = Vector3D(1, 0, 0);
	}

	Right = Vector3D::Cross(normal, arbitraryUp).Normalised();

	Up = Vector3D::Cross(Right, normal).Normalised();

	PlaneCenter = (CurrentPoint + closestPoint) / 2;
}

bool Voronoi::IsPointInPolygon(Vector3D Point, Array<Vector3D> Polygon, Vector3D center)
{
	size_t t = 0;

	for (size_t i = 0; i < Polygon.GetSize(); i++)
	{
		//Vector3D t = { Polygon[i] - Polygon[(i + 1) % Polygon.GetSize()] };
		//Vector3D u = { Point - Polygon[(i + 1) % Polygon.GetSize()] };
		//Vector3D v = { Polygon[(i + 2) % Polygon.GetSize()] - Polygon[(i + 1) % Polygon.GetSize()]  };

		//if (!(Vector3D::Cross(t, u) * Vector3D::Cross(t, v) >= Vector3D::Zero && Vector3D::Cross(v, u) * Vector3D::Cross(v, t) >= Vector3D::Zero)) return false;

		float d = Vector3D::Dot(Point, (Polygon[i] - center).Normalised());

		if (d < 0 && !MathCore::IsNearlyZero(d)) t++ ;

		std::cout << t << "\n";

	}

	if (t > 0)return false;

	return true;
}

FracturePiece3D::FracturePiece3D(Array<Vector3D> cell, Vector3D Point)
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

	for (Vector3D vert : cell)
	{
		Verts.Add(vert.X);
		Verts.Add(vert.Y);
		Verts.Add(vert.Z);
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
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

	glBindVertexArray(0);

	//glGenVertexArrays(1, &PVAO);
	//glGenBuffers(1, &PVBO);

	//glBindVertexArray(PVAO);

	//glBindBuffer(GL_ARRAY_BUFFER, PVBO);

	//Array<float> Points = { Point.X, Point.Y };

	//glBufferData(GL_ARRAY_BUFFER, 2 * sizeof(float), Points.GetFirstRef(), GL_STATIC_DRAW);

	//glEnableVertexAttribArray(0);
	//glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);


	Renderer::FracturesToDraw3D.Add(this);
}

void FracturePiece3D::Draw(const Shader* InShader)
{
	InShader->Use();
	InShader->SetVec3("Color", color);

	//std::cout << "DrawCalled";
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	//TODO find a way to separate this from model
	//used for reflection InShade
	glPointSize(5);
	glBindVertexArray(VAO);
	//glBindBuffer(GL_ARRAY_BUFFER, VBO);
	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(Inds.GetSize()), GL_UNSIGNED_INT, nullptr);
	glBindVertexArray(0);
	glUseProgram(0);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	/*PointShader.Use();
	glBindVertexArray(PVAO);*/
	//glBindBuffer(GL_ARRAY_BUFFER, VBO);
	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	//glDrawArrays(GL_POINTS, 0, 1);
	//glBindVertexArray(0);
	//glUseProgram(0);
}

