#include "Fracturerer.h"
#include "WireShapes.h"
#include "Array.h"

void Fracturerer::Fracture(Model& InModel, const unsigned int FracturePoints)
{
	Array<Vector3D> points;

	for (unsigned int i = 0; i < FracturePoints; i++)
	{
		points.Add(InModel.ModelTransform.GetRandomPointInBounds());
		DrawWireCube(points[i], Vector3D(1, 1, 1), Vector3D(0.1, 0.1, 0.1), Vector3D(0.2, 0.5, 0.2));
	}

	Vector3D closestPoint = points[1];
	Vector3D CurrentPoint = points[0];

	for (unsigned int i= 2; i < points.GetSize(); i++)
	{
		const float distance = (points[i] - CurrentPoint).GetSquaredLength();

		if (distance < closestPoint.GetSquaredLength())
		{
			closestPoint = points[i];
		}
	}



	Vector3D normal = (CurrentPoint - closestPoint).Normalised();

	Vector3D arbitraryUp = Vector3D::Up;
	if (fabs(Vector3D::Dot(normal, arbitraryUp)) > 0.99f) {
		arbitraryUp = Vector3D(1, 0, 0);  // pick another axis if too close
	}

	Vector3D Right = Vector3D::Cross(normal, arbitraryUp).Normalised();

	Vector3D Up = Vector3D::Cross(Right, normal).Normalised();

	if (Vector3D::Dot(normal, Up) == 0)
	{
		std::cout << "Success" << std::endl;
	}
	else
	{
		std::cout << Vector3D::Dot(normal, Up) << std::endl;
	}

	DrawWirePlane(CurrentPoint, normal, Vector3D(10, 10, 10), Vector3D(0.5, 0.1, 0.1));

	for (unsigned int i = 0; i < InModel.BoundingBox->Faces.GetSize(); i++)
	{
		for(unsigned int edge = 0; edge < 2; edge++)
		{
			Vector3D MeasuredVert = InModel.BoundingBox->Faces[i].Verticies[edge].Position;

			Vector3D dirToNext = (InModel.BoundingBox->Faces[i].Verticies[edge + 1].Position - MeasuredVert).Normalised();

			Vector3D intersectionPoint;

			if (Vector3D::GetIntersectionPoint(CurrentPoint, Right,MeasuredVert, dirToNext, intersectionPoint))
			{
				DrawWireCube(intersectionPoint, Vector3D(1, 1, 1), Vector3D(0.1, 0.1, 0.1), Vector3D(0.1, 0.1, 0.5));
			}

			//Vector3D intersectionPoint;

			if (Vector3D::GetIntersectionPoint(CurrentPoint, Up, MeasuredVert, dirToNext, intersectionPoint))
			{
				DrawWireCube(intersectionPoint, Vector3D(1, 1, 1), Vector3D(0.1, 0.1, 0.1), Vector3D(0.1, 0.1, 0.5));
			}

		}
	}



	InModel.BoundingBox->Initialise();


}
