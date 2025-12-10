
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

void Voronoi::GetFirstIntersection(const Vector3D& Normal, const Vector3D& Center, const Face& CurrentFace, Face& NewFace, size_t& FirstIntersectionIndex, Vector3D& FirstIntersection)
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

size_t Voronoi::GetAllVertsUntilSecondIntersection(const Vector3D& Normal, const Vector3D& Center, const Face& CurrentFace, Face& NewFace, const size_t&
                                                   FirstIntersectionIndex, Vector3D& SecondIntersection)
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

void Voronoi::GetFaceReveresed(Face& IntersectFace, const Face& CurrentFace, Face& NewFace, const size_t& FirstIntersectionIndex, const Vector3D&
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

	bool bAddFirst = true, bAddSecond = true;
	for (const auto& vert : IntersectFace.Vertices)
	{
		if (Vector3D::IsAlmostEqual(vert, FirstIntersection)) bAddFirst = false;

		if (Vector3D::IsAlmostEqual(vert, SecondIntersection)) bAddSecond = false;
	}

	if (bAddSecond) IntersectFace.Vertices.Add(SecondIntersection);
	if (bAddFirst) IntersectFace.Vertices.Add(FirstIntersection);
}

void Voronoi::SliceFaceByPlane(const Array<Face>& Faces, const Vector3D& Normal, const Vector3D& Center, Array<Face>& NewFaces,
                               Face& IntersectFace, const size_t& FaceIndex)
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
			NewFaces.Add(currentFace);
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
			IntersectFace,
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
		for (const auto& vert : IntersectFace.Vertices)
		{
			if (Vector3D::IsAlmostEqual(vert, firstIntersection)) bAddFirst = false;

			if (Vector3D::IsAlmostEqual(vert, secondIntersection)) bAddSecond = false;
		}

		if (bAddFirst) IntersectFace.Vertices.Add(firstIntersection);

		if (bAddSecond) IntersectFace.Vertices.Add(secondIntersection);
	}

	NewFaces.Add(newFace);
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
		
		Vector3D point1;
		do 
		{
			point1 = InModel.ModelTransform.GetRandomPointInBounds();
		} while (points.Contains(point1) && IsPointTooClose(point1, points));

		//TestSquare.push_back(DrawWireCube(point1, { 0.5, 0.5, 0.5 }, { 0.1f, 0.1f, 0.1f }, { 0.5, 0.5, 0.5 }));

		points[i] = point1;
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


		//for (auto& face : Faces)
		//{
		//	FracturePiece3D* frac = CreateObjectPtr<FracturePiece3D>(face.Vertices, currentPoint);
		//	frac->color = color;
		//	fractureFaces.Add(face);
		//}
//

        FracturePiece3D* frac = CreateObjectPtr<FracturePiece3D>(Faces, currentPoint);
		frac->color = color;
	//	OutFractures.Add(frac);


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

bool Voronoi::IsPointInPolygon(const Vector3D& Point, const Array<Vector3D>& Polygon, const Vector3D& center)
{
	//Check if any point is on the wrong side of the plane, i.e. on the negative side of the normal
	for (size_t i = 0; i < Polygon.GetSize(); i++)
	{
		float d = Vector3D::Dot(Point, (Polygon[i] - center).Normalised());

		if (d < 0 && !MathCore::IsNearlyZero(d)) return false ;

	}
	return true;
}

bool Voronoi::IsPointTooClose(const Vector3D& Point, const Array<Vector3D>& Points)
{
	for (const auto& p : Points)
	{
		double d = (Point - p).GetSquaredLength();
		if (d < 0.001) return true;

	}

	return false;
}


FracturePiece3D::~FracturePiece3D()
{
	
}

FracturePiece3D::FracturePiece3D(Array<Face> cell, Vector3D Point)
{/*
	InputManager* inputManager = Camera::GetActiveCamera()->GetActiveInputManager();
	auto LeftArrow = std::make_unique<InputAction>(GLFW_KEY_LEFT, inputManager, Camera::GetActiveWindow());

	LeftArrow->Actions.BindMember(this, &FracturePiece3D::Seperate);

	auto RightArrow = std::make_unique<InputAction>(GLFW_KEY_RIGHT, inputManager, Camera::GetActiveWindow());

	RightArrow->Actions.BindMember(this, &FracturePiece3D::Converge);*/

	dir = (Point - Vector3D::Zero).Normalised();

	shader = Shader("ColorShape", "/Shaders/");

	Array<Vector3D> newVerts;

	dir = Vector3D::RandomRange(Vector3D::Zero, Vector3D(100, 100, 100));

	dir = dir.Normalised();


	for (const auto& face : cell)
	{
		for (size_t i = 1; i + 1 < face.Vertices.GetSize(); i++)
		{
			size_t index = 0;
			if (newVerts.Contains(face.Vertices[0], index))
			{
				Inds.Add(index);
			}
			else
			{
				Inds.Add(newVerts.GetSize());
				newVerts.Add(face.Vertices[0]);
			}

			if (newVerts.Contains(face.Vertices[i], index))
			{
				Inds.Add(index);
			}
			else
			{
				Inds.Add(newVerts.GetSize());
				newVerts.Add(face.Vertices[i]);
			}

			if (newVerts.Contains(face.Vertices[i + 1], index))
			{
				Inds.Add(index);
			}
			else
			{
				Inds.Add(newVerts.GetSize());
				newVerts.Add(face.Vertices[i+1]);
			}

		}
	}

	
	for (const Vector3D& vert : newVerts) {
		Verts.Add(vert.X);
		Verts.Add(vert.Y);
		Verts.Add(vert.Z);
	}

	////for (size_t i = 1; i + 1< cell.GetSize(); i++)
	////{
	////	Inds.Add(0);
	////	Inds.Add(i);
	////	Inds.Add(i + 1);
	////}


	if (Inds.IsEmpty())
	{
		return;
	}

	::DataBuffers::GenBuffer(VAO);


	DataBuffers::BindVertexInfo(VAO, 0, 0, sizeof(Vector3D), 0, Vector3);


	::DataBuffers::BufferData(VAO, Verts.GetSize() * sizeof(float), Verts.GetFirstPtr(), BufferTargets::VERTEX);
	DataBuffers::BufferDataIndex(VAO, Inds.GetSize() * sizeof(uint16_t), Inds.GetFirstPtr());



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

	shader.SetUniformBuffer(0, &g, sizeof(GlobalTransforms));

	shader.SetUniformBuffer(1, &ubo, sizeof(PerInstanceTransforms));

	::Renderer::Draw(Inds.GetSize());

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

void FracturePiece3D::Seperate()
{

	transform.Position += dir;

}
void FracturePiece3D::Converge()
{
	transform.Position -= dir;
}

