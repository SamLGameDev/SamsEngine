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

	for (unsigned int i = 0; i < FracturePoints; i++)
	{
		std::cout << "done: " << InModel.BoundingBox->Indices.GetSize() << std::endl;
		Vector3D point = points[i];

		Array<unsigned int> CurrentInds = InModel.BoundingBox->Indices;

		Array<Vertex> CurrentPoints = InModel.BoundingBox->Vertices;

		for (unsigned int j = 0; j < FracturePoints; j++)
		{
			if (i == j)
			{
				continue;
			}

			const Vector3D dirToOther = (points[j] - point).Normalised();
			const Vector3D mid = (point + points[j]) / 2;

			Array<Vertex> CP = CurrentPoints;


			for (unsigned int vertex = 0; vertex < CurrentPoints.GetSize(); vertex ++)
			{
				WireObject* BoundingBox = InModel.BoundingBox;

				//Array<Vector3D> verts;
				//for (unsigned int v = 0; v < 3; v++)
				//{
				//	verts.Add(BoundingBox->Vertices[BoundingBox->Indices[vertex + v]].Position);
				//}

				float edge = vertex;

					const float IsFirstOnPlane = Vector3D::Dot(dirToOther, CurrentPoints[edge].Position - mid);

					unsigned int next = edge < CurrentPoints.GetSize() ? vertex + 1: 0;

					const float IsSecondOnPlane = Vector3D::Dot(dirToOther, CurrentPoints[next].Position - mid);

					const bool Inside = IsFirstOnPlane >= 0;

					const bool Inside2 = IsSecondOnPlane >= 0;

		

					if (Inside != Inside2)
					{
						const float tStar = IsFirstOnPlane / (IsFirstOnPlane - IsSecondOnPlane);

						const Vector3D intersectionPoint = CurrentPoints[edge].Position + tStar * (CurrentPoints[next].Position - CurrentPoints[edge].Position);

						Vertex vert;
						vert.Position = intersectionPoint;
						vert.Color = Vector3D(0.2, 0.5, 0.2);

						unsigned int index;

						if (!CP.Contains(vert, index)) 
						{
							CurrentPoints.Add(vert);
						}


						//if (BoundingBox->Vertices.Contains(vert, index))
						//{
						//	//BoundingBox->Indices.Add(index);
						//}
						//else
						//{
						//	BoundingBox->Indices.Add(BoundingBox->Vertices.GetSize());
						//	BoundingBox->Vertices.Add(vert);
						//}


					}
					else 
					{
						Vertex vert = CurrentPoints[edge];

						CP.Remove(vert);
					}

					if (Inside2)
					{
						Vertex vert;
						vert.Position = CurrentPoints[next].Position;
						vert.Color = Vector3D(0.2, 0.5, 0.2);


						unsigned int index;

						if (!CP.Contains(vert, index)) 
						{
							CP.Add(vert);
						}

						//if (BoundingBox->Vertices.Contains(vert, index))
						//{
						//	BoundingBox->Indices.Add(index);
						//}
						//else
						//{
						//	BoundingBox->Indices.Add(BoundingBox->Vertices.GetSize());
						//	BoundingBox->Vertices.Add(vert);
						//}
					}
					else 
					{
						Vertex vert = CurrentPoints[next];
						CP.Remove(vert);
					}
		
			}
			CurrentPoints = CP;


		}
		for (unsigned int k = 0; k < CurrentPoints.GetSize(); k++)
		{
			InModel.BoundingBox->Vertices.Add(CurrentPoints[k]);
			InModel.BoundingBox->Indices.Add(InModel.BoundingBox->Vertices.GetSize());

			DrawWireCube(CurrentPoints[k].Position, Vector3D(1, 1, 1), Vector3D(0.1, 0.1, 0.1), Vector3D(0.5, 0.1, 0.1));

			std::cout << CurrentPoints[k].Position.X << CurrentPoints[k].Position.Y << CurrentPoints[k].Position.Z << std::endl;
		}

	}
	InModel.BoundingBox->Initialise();


}
