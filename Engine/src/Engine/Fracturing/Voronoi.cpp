
#include "Voronoi.h"
#include "transform.h"
#include <iostream>

void Voronoi::FracturePlaneRandom(Model& InModel)
{
	Vector3D center = InModel.BoundingBox->WireTransform.TransCenter;

	Array<Vector3D> points;

	//for (size_t i = 0; i < 100; i++)
	//{
	//	Vector3D point1 = InModel.ModelTransform.GetRandomPointInBounds();

	//	while (points.Contains(point1))
	//	{
	//		point1 = InModel.ModelTransform.GetRandomPointInBounds();
	//	}
	//	TestSquare.push_back(DrawWireCube(point1, { 0.5, 0.5, 0.5 }, { 0.1f, 0.1f, 0.1f }, { 0.5, 0.5, 0.5 }));

	//	points.Add(point1);
	//}

	points = { {0.1, 0.1, 0.1}, {0.8, 0.4, 0.2} };

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

			DefinePlane(normal, currentPoint, comparedPoint, right, up, center);

			for (size_t f = 0; f < Faces.GetSize(); f++)
			{
				Face currentFace = Faces[f];
				Face newFace;

				for (size_t p = 0; p < currentFace.Vertices.GetSize(); p++)
				{
					Vector3D fromVert = currentFace.Vertices[p];
					Vector3D toVert = currentFace.Vertices[(p + 1) % currentFace.Vertices.GetSize()];

					Vector3D lineDir = toVert - fromVert;
					Vector3D intersectionPoint;
					const bool bDoesLineIntersect = Vector3D::GetIntersectionPointWithPlane(center, normal, fromVert, lineDir, intersectionPoint);

					if (bDoesLineIntersect)
					{
						std::cout << "Start: \n";
						fromVert.Print();
						toVert.Print();
						intersectionPoint.Print();
						newFace.Vertices.Add(intersectionPoint);
						break;
						
					}

				}
				if (newFace.Vertices.IsEmpty())newFace = currentFace;
				else
				{
					for ()
				}
			}
			

		}


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
