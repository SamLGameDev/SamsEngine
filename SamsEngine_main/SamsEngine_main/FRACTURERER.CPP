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
		std::cout << "done: " << InModel.BoundingBox->Indices.GetSize()<< std::endl;
		Vector3D point = points[i];

		Array<Face> faces = InModel.BoundingBox->Faces;

		for (unsigned int j = 0; j < FracturePoints; j++)
		{
			if (i == j)
			{
				continue;
			}

			const Vector3D dirToOther = (point - points[j]).Normalised();
			const Vector3D mid = (point + points[j]) / 2;

			Array<unsigned int> CurrentInds = InModel.BoundingBox->Indices;

			Array<Face> newFaces;

			for (unsigned int f = 0; f < faces.GetSize(); f++)
			{
				Array<Vertex> Verticies = faces[f].Verticies;

				const float EQS = 1e-6f;

				if (Verticies.GetSize() == 0)
				{
					continue;
				}

				Array<Vertex> Copy;

				for (unsigned int n = 0; n < Verticies.GetSize(); n++)
				{
					const unsigned int next = (n + 1) % Verticies.GetSize();

					const float dp = Vector3D::Dot(dirToOther, Verticies[n].Position - mid);

					const float dq = Vector3D::Dot(dirToOther, Verticies[next].Position - mid);

					const bool InsideDp = dp >= 0;

					const bool InsideDq = dq >= 0;

					if (InsideDp && InsideDq)
					{
						Copy.Add(Verticies[next]);
					}
					else if (InsideDp && !InsideDq)
					{
						const float denom = dp - dq;

						if (fabs(denom) > EQS)
						{
							const float t = dp / denom;
							Vector3D I = Verticies[n].Position + t * (Verticies[next].Position - Verticies[n].Position);
							Vertex v = Verticies[n];
							v.Position = I;
							std::cout << "Dp: " << I.X << std::endl;

							Copy.Add(v);
						}
					}
					else if (!InsideDp && InsideDq)
					{
						const float denom = dp - dq;

						if (fabs(denom) > EQS)
						{
							const float t = dp / denom;
							Vector3D I = Verticies[n].Position + t * (Verticies[next].Position - Verticies[n].Position);
							Vertex v = Verticies[n];
							v.Position = I;
							std::cout << "Dq: " << I.X << std::endl;

							Copy.Add(v);
						}
						Copy.Add(Verticies[next]);
					}
				}

				if (Copy.GetSize() >= 3)
				{
					for (unsigned int k = 1; k + 1 < Copy.GetSize(); k++)
					{
						Face tri;
						tri.Verticies.Add(Copy[0]);
						tri.Verticies.Add(Copy[k]);
						tri.Verticies.Add(Copy[k + 1]);
						newFaces.Add(tri);
					}
				}

			}
			faces = newFaces;
			if (faces.GetSize() == 0)
			{
				break;
			}
		

		}

		for (unsigned int poly = 0; poly < faces.GetSize();poly++)
		{
			if (faces[poly].Verticies.GetSize() < 3)
			{
				continue;
			}

			Array<unsigned int> CopyInds;

			for (unsigned int v = 0; v < faces[poly].Verticies.GetSize(); v++)
			{
				unsigned int index = 0;

				if (InModel.BoundingBox->Vertices.Contains(faces[poly].Verticies[v], index))
				{
					CopyInds.Add(index);
				}
				else
				{
					CopyInds.Add(InModel.BoundingBox->Vertices.GetSize());
					InModel.BoundingBox->Vertices.Add(faces[poly].Verticies[v]);
					DrawWireCube(faces[poly].Verticies[v].Position, Vector3D(1,1,1), Vector3D(0.1, 0.1, 0.1), Vector3D(0.1, 0.1, 0.5));
				}
			}

			for (unsigned int k = 1; k + 1 < CopyInds.GetSize(); k++)
			{
				InModel.BoundingBox->Indices.Add(CopyInds[0]);
				InModel.BoundingBox->Indices.Add(CopyInds[k]);
				InModel.BoundingBox->Indices.Add(CopyInds[k + 1]);
			}
		}
	}

	InModel.BoundingBox->Initialise();


}
