
#include "Voronoi.h"
#include "transform.h"
#include <iostream>

#include "Camera.h"
#include "DataBuffers.h"
#include "DelaunayTriangulation.h"
#include "MathCore.h"
#include "ObjectFactory.h"
#include "InterfaceRenderer.h"
#include "glm/gtc/type_ptr.hpp"

void Voronoi::GetFirstIntersection(Vector3D normal, Vector3D center, const Face& currentFace, Face newFace, size_t& firstIntersectionIndex, Vector3D& firstIntersection)
{
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
}

size_t Voronoi::GetAllVertsUntilSecondIntersection(Vector3D normal, Vector3D center, const Face& currentFace, Face newFace, size_t firstIntersectionIndex, Vector3D& secondIntersection)
{
	size_t secondIntersectionIndex = 0;

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
	return secondIntersectionIndex;
}

void Voronoi::GetFaceReveresed(Face intersectFace, const Face& currentFace, Face newFace, size_t firstIntersectionIndex, Vector3D firstIntersection, Vector3D secondIntersection, size_t secondIntersectionIndex)
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

void Voronoi::SliceFaceByPlane(Array<Face>& Faces, Vector3D& Normal, Vector3D& Center, Array<Face>& newFaces,
                               Face& intersectFace, const size_t& FaceIndex)
{
	const Face& currentFace = Faces[FaceIndex];
	Face newFace;

	size_t firstIntersectionIndex = 0;
	Vector3D firstIntersection;

	GetFirstIntersection(Normal, Center, currentFace, newFace, firstIntersectionIndex, firstIntersection);
		
	//Check if there is a first intersection, if not, 
	//check if face is on the current points side of the plane, and if so, keep it

	if (newFace.Vertices.IsEmpty())
	{
		if (Vector3D::Dot(Normal, (currentFace.Vertices[0] - Center).Normalised()) > 0)
		{
			newFaces.Add(currentFace);
		}
		return;
	}

	//Get all verts up to and including the second intersection

	Vector3D secondIntersection;
	size_t secondIntersectionIndex = GetAllVertsUntilSecondIntersection
	(
		Normal,
		Center,
		currentFace,
		newFace,
		firstIntersectionIndex,
		secondIntersection
	);

	//Is the point inside the polygon formed by the new face? if not, reverse it by
	//starting from the second intersection and continuing until the first intersection

	if (!IsPointInPolygon(Normal, newFace.Vertices, Center))
	{
		GetFaceReveresed
		(
			intersectFace,
			currentFace,
			newFace,
			firstIntersectionIndex,
			firstIntersection,
			secondIntersection,
			secondIntersectionIndex
		);
	}
	else
	{

		// if the intersections arent already in the intersect face, add them

		bool bAddFirst = true, bAddSecond = true;
		for (auto vert : intersectFace.Vertices)
		{
			if (Vector3D::IsAlmostEqual(vert, firstIntersection)) bAddFirst = false;

			if (Vector3D::IsAlmostEqual(vert, secondIntersection)) bAddSecond = false;
		}

		if (bAddFirst) intersectFace.Vertices.Add(firstIntersection);

		if (bAddSecond) intersectFace.Vertices.Add(secondIntersection);
	}
				

	newFaces.Add(newFace);
	return;
}

void Voronoi::SliceShapeByPlane(const Array<Vector3D>& Points, const size_t& Index, Vector3D& CurrentPoint, Array<Face>& Faces,
                                Vector3D& Normal, Vector3D& Right, Vector3D& Up, Vector3D& Center, const size_t& J)
{
	if (J == Index)return;

	Vector3D comparedPoint = Points[J];

	Array<Face> newFaces;

	Face intersectFace;

	DefinePlane(Normal, CurrentPoint, comparedPoint, Right, Up, Center);

	//Slice each face by plane
	for (size_t f = 0; f < Faces.GetSize(); f++)
	{
		SliceFaceByPlane(Faces, Normal, Center, newFaces, intersectFace, f);
	}

	//if there is a valid intersect face, add it to the list of faces
	if (intersectFace.Vertices.GetSize() > 2) newFaces.Add(intersectFace);

	Faces = newFaces;
}

void Voronoi::FracturePlaneRandom(Model& InModel, Array<FracturePiece3D>& OutFractures, const size_t& NumPoints)
{



	Array<Vector3D> points(NumPoints);

	for (size_t i = 0; i < NumPoints; i++)
	{
		Vector3D point1 = InModel.ModelTransform.GetRandomPointInBounds();

		while (points.Contains(point1))
		{
			point1 = InModel.ModelTransform.GetRandomPointInBounds();
		}

		//TestSquare.push_back(DrawWireCube(point1, { 0.5, 0.5, 0.5 }, { 0.1f, 0.1f, 0.1f }, { 0.5, 0.5, 0.5 }));

		points.Add(point1);
	}

	//TestPoints
	//points = { {0.1, 0.1, 0.1}, {0.8, 0.4, 0.2}, {0.4, 0.8, 0.6}, {0.6, 0.2, 0.1} };
	//points = { {0.1, 0.1, 0.1}, {0.8, 0.4, 0.2} };


	for (size_t i = 0; i < points.GetSize(); i++)
	{
		Vector3D currentPoint = points[i];

		Array<Face> Faces = InModel.BoundingBox->Faces;

		Vector3D normal, right, up, center;

		for (size_t j = 0; j < points.GetSize(); j++)
		{
			SliceShapeByPlane(points, i, currentPoint, Faces, normal, right, up, center, j);
		}

		auto color = Vector3D::RandomRange(Vector3D(30, 30, 30), Vector3D(255, 255, 255));


		////for (auto& face : Faces)
		////{

		////	FracturePiece3D* frac = CreateObjectPtr<FracturePiece3D>(face.Vertices, currentPoint);
		////	frac->color = color;
		////	fractureFaces.Add(face);
		////}
//
		Array<Vector3D> test;
		for (auto& face : Faces)
		{
	
			test.Add(face.Vertices);

		}
        FracturePiece3D frac = CreateObjectRaw<FracturePiece3D>(test, currentPoint);
		frac.color = color;
		OutFractures.Add(frac);


		//TestSquare.push_back(DrawWireCube(currentPoint, { 0.5, 0.5, 0.5 }, { 0.1f, 0.1f, 0.1f }, color/255));

	}

}
void Voronoi::DefinePlane(Vector3D& normal, Vector3D& CurrentPoint, Vector3D& closestPoint, Vector3D& Right, Vector3D& Up, Vector3D& PlaneCenter)
{
	normal = (CurrentPoint - closestPoint).Normalised();
	
	//if normal is nearly parallel to up vector, use right vector instead
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
	//Check if any point is on the wrong side of the plane, i.e. on the negative side of the normal
	for (size_t i = 0; i < Polygon.GetSize(); i++)
	{
		float d = Vector3D::Dot(Point, (Polygon[i] - center).Normalised());

		if (d < 0 && !MathCore::IsNearlyZero(d)) return false ;

	}
	return true;
}

FracturePiece3D::~FracturePiece3D()
{
	
}

FracturePiece3D::FracturePiece3D(Array<Vector3D> cell, Vector3D Point)
{

	shader = Shader("ColorShape", "/Shaders/");

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

	dir = Vector3D::RandomRange(Vector3D::Zero, Vector3D(100, 100, 100));

	dir = dir.Normalised();

	//std::cout << "Inds: ";
	for (size_t i = 1; i + 1 < cell.GetSize(); i++)
	{
		Inds.Add(0);
		Inds.Add(i);
		Inds.Add(i + 1);
	}


	//DelaunayTriangulation triangulation;

	//triangulation.Triangulate(cell, Inds);

	if (Inds.IsEmpty())
	{
		return;
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

	::DataBuffers::GenBuffer(VAO);


	DataBuffers::BindVertexInfo(VAO, 0, 0, sizeof(Vector3D), 0, Vector3);

	//::DataBuffers::BindBuffer(VAO);


	::DataBuffers::BufferData(VAO, Verts.GetSize() * sizeof(float), Verts.GetFirstPtr(), BufferTargets::VERTEX);
	DataBuffers::BufferDataIndex(VAO, Inds.GetSize() * sizeof(uint16_t), Inds.GetFirstPtr());

	//glGenVertexArrays(1, &VAO);
	//glGenBuffers(1, &VBO);
	//glGenBuffers(1, &EBO);

	//glBindVertexArray(VAO);

	//glBindBuffer(GL_ARRAY_BUFFER, VBO);

	//glBufferData(GL_ARRAY_BUFFER, Verts.GetSize() * sizeof(float), Verts.GetFirstPtr(), GL_STATIC_DRAW);

	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

	//glBufferData(GL_ELEMENT_ARRAY_BUFFER, Inds.GetSize() * sizeof(int), Inds.GetFirstPtr(), GL_STATIC_DRAW);

	//glEnableVertexAttribArray(0);
	//glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

	//glBindVertexArray(0);

	//glGenVertexArrays(1, &PVAO);
	//glGenBuffers(1, &PVBO);

	//glBindVertexArray(PVAO);

	//glBindBuffer(GL_ARRAY_BUFFER, PVBO);

	//Array<float> Points = { Point.X, Point.Y };

	//glBufferData(GL_ARRAY_BUFFER, 2 * sizeof(float), Points.GetFirstPtr(), GL_STATIC_DRAW);

	//glEnableVertexAttribArray(0);
	//glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);


	::Renderer::AddFracture(this);
}

void FracturePiece3D::Draw()
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


	//Projection[1][1] *= -1;
	shader.SetUniformBuffer(0, &g, sizeof(GlobalTransforms));

	shader.SetUniformBuffer(1, &ubo, sizeof(PerInstanceTransforms));

	::Renderer::Draw(Inds.GetSize());

	//std::cout << "DrawCalled";
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	//TODO find a way to separate this from model
	//used for reflection InShade
	//glPointSize(5);
	//glBindVertexArray(VAO);
	//glBindBuffer(GL_ARRAY_BUFFER, VBO);
	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	//glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(Inds.GetSize()), GL_UNSIGNED_INT, nullptr);
	//glBindVertexArray(0);
	//glUseProgram(0);
	//glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	/*PointShader.Use();
	glBindVertexArray(PVAO);*/
	//glBindBuffer(GL_ARRAY_BUFFER, VBO);
	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	//glDrawArrays(GL_POINTS, 0, 1);
	//glBindVertexArray(0);
	//glUseProgram(0);
}

void FracturePiece3D::Start()
{
	WorldObject::Start();
}

void FracturePiece3D::Tick(const double& DeltaTime)
{
	WorldObject::Tick(DeltaTime);

	//if (glfwGetTime() > 10)
	//{

	//	transform.Position = transform.Position + (dir * 5) * DeltaTime;
	//}

}

