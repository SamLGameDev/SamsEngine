#include "Fracturerer.h"
#include "WireShapes.h"
#include "Array.h"
#include "Camera.h"
#include "glm-1.0.1/glm/gtc/type_ptr.hpp"
#include "Renderer.h"

//TO-DO
// remake this to use seperate objects for each fracture, this way we dont need to worry about re-meshing the mesh

void Fracturerer::Fracture(Model& InModel, const unsigned int FracturePoints)
{
	Array<Vector3D> points;

	for (unsigned int i = 0; i < FracturePoints; i++)
	{
		points.Add(InModel.ModelTransform.GetRandomPointInBounds());
		DrawWireCube(points[i], Vector3D(1, 1, 1), Vector3D(0.1, 0.1, 0.1), Vector3D(0.2, 0.5, 0.2));
	}

	for (unsigned int p = 0; p < points.GetSize(); p++)
	{



		Array<Vector3D> remainingPoints = points;
		Vector3D closestPoint;
		Vector3D CurrentPoint = points[p];


		Array<Vector3D> newPoints;

		Array<Face> CurrentFaces = InModel.BoundingBox->Faces;


		while (remainingPoints.GetSize() > 1)
		{
			closestPoint = points[(p + 1) % points.GetSize()];

			GetClosestPoint(remainingPoints, points, CurrentPoint, closestPoint);

			Vector3D normal, Right, Up;

			DefinePlane(normal, CurrentPoint, closestPoint, Right, Up);
			//(CurrentPoint, CurrentPoint + (normal * 10), Vector3D(0.1, 0.1, 0.6));
		//	std::cout << normal.X << ", " << normal.Y << ", " << normal.Z << std::endl;

			CullPoints(remainingPoints, CurrentPoint, normal);

			Face UnfinishedFace;

			Vector3D first;

			bool bFirst = false;

			Array<Face> CopyFace = CurrentFaces;

			for (unsigned int i = 0; i < CurrentFaces.GetSize(); i++)
			{
				Face face;
				Array<Vector3D> IPs;
				for (unsigned int edge = 0; edge < 2; edge++)
				{
					Array<Vertex> verts = CurrentFaces[i].Verticies;

					Vector3D LineOriginWorld = verts[edge].Position;

					glm::vec4 glmLine = glm::vec4(LineOriginWorld.X, LineOriginWorld.Y, LineOriginWorld.Z, 1);

					glmLine = InModel.BoundingBox->WireTransform.GetModelMatrix() * glmLine;

					LineOriginWorld = Vector3D(glmLine.x, glmLine.y, glmLine.z);

					glmLine = glm::vec4(verts[edge + 1].Position.X, verts[edge + 1].Position.Y, verts[edge + 1].Position.Z, 1);

					glmLine = InModel.BoundingBox->WireTransform.GetModelMatrix() * glmLine;

					Vector3D lineDIr = (Vector3D(glmLine.x, glmLine.y, glmLine.z) - LineOriginWorld);

					Vector3D intersectionPoint;

					if (Vector3D::GetIntersectionPoint(CurrentPoint, normal, LineOriginWorld, lineDIr, intersectionPoint))
					{
						glm::vec4 glmIntersectionPoint = glm::vec4(intersectionPoint.X, intersectionPoint.Y, intersectionPoint.Z, 1);
						glm::vec4 ToLocal = InModel.BoundingBox->WireTransform.GetInverseModelMatrix() * glmIntersectionPoint;

						DrawWireCube(Vector3D(glmIntersectionPoint.x, glmIntersectionPoint.y, glmIntersectionPoint.z), Vector3D(1, 1, 1), Vector3D(0.1, 0.1, 0.1), Vector3D(0.5, 0.5, 0.5));

						IPs.Add(Vector3D(ToLocal.x, ToLocal.y, ToLocal.z));

						if (!UnfinishedFace.Verticies.IsEmpty())
						{
							Vertex v;
							v.Position = Vector3D(ToLocal.x, ToLocal.y, ToLocal.z);

							UnfinishedFace.Verticies.Add(v);
							CopyFace.Add(UnfinishedFace);
							UnfinishedFace = Face();
						}

						if (!bFirst)
						{
							first = IPs[0];
							bFirst = true;
						}

						//Vertex v;
						//v.Position = Vector3D(ToLocal.x, ToLocal.y, ToLocal.z);

					}



				}

				if (!IPs.IsEmpty())
				{
					face.Verticies.Add(CurrentFaces[i].Verticies[0]);
					for (unsigned int k = 0; k < IPs.GetSize(); k++)
					{
						Vertex v;
						v.Position = IPs[k];

						face.Verticies.Add(v);
					}

					if (face.Verticies.GetSize() < 3)
					{
						UnfinishedFace = face;
						Face CurrentFace = CurrentFaces[i];
						CopyFace.Remove(CurrentFace);

					}
					else
					{
						CopyFace.Add(face);
					}
				}
				else
				{
					Face CurrentFace = CurrentFaces[i];
					CopyFace.Remove(CurrentFace);
				}

			}

			if (!UnfinishedFace.Verticies.IsEmpty())
			{
				Vertex v;
				v.Position = first;

				UnfinishedFace.Verticies.Add(v);

				CopyFace.Add(UnfinishedFace);
			}
			CurrentFaces = CopyFace;
		}

		Vertex point;
		point.Color = Vector3D::RandomRange(Vector3D::Zero, Vector3D(1,1,1));
		for (unsigned int j = 0; j < CurrentFaces.GetSize(); j++)
		{
			unsigned int ind = InModel.BoundingBox->Vertices.GetSize();

			point.Position = CurrentFaces[j].Verticies[0].Position;

			InModel.BoundingBox->Vertices.Add(point);

			point.Position = CurrentFaces[j].Verticies[1].Position;

			InModel.BoundingBox->Vertices.Add(point);

			point.Position = CurrentFaces[j].Verticies[2].Position;

			InModel.BoundingBox->Vertices.Add(point);

			InModel.BoundingBox->Indices.Add(ind);
			InModel.BoundingBox->Indices.Add(ind + 1);
			InModel.BoundingBox->Indices.Add(ind + 2);
		}

	}


	//for (unsigned int i = 0; i + 2 < newPoints.GetSize(); i+=3)
	//{
	//	unsigned int ind = InModel.BoundingBox->Vertices.GetSize();

	//	Vertex point;
	//	point.Color = Vector3D(0.5, 0.1, 0.1);
	//	point.Position = newPoints[i];

	//	InModel.BoundingBox->Vertices.Add(point);

	//	point.Position = newPoints[i +1];

	//	InModel.BoundingBox->Vertices.Add(point);	

	//	point.Position = newPoints[i +2];

	//	InModel.BoundingBox->Vertices.Add(point);

	//	InModel.BoundingBox->Indices.Add(ind);

	//	InModel.BoundingBox->Indices.Add(ind+1);
	//	
	//	InModel.BoundingBox->Indices.Add(ind + 2);
	//	

	//}


	InModel.BoundingBox->Initialise();


}

void Fracturerer::DefinePlane(Vector3D& normal, Vector3D& CurrentPoint, Vector3D& closestPoint, Vector3D& Right, Vector3D& Up)
{
	normal = (CurrentPoint - closestPoint).Normalised();


	Vector3D arbitraryUp = Vector3D::Up;
	if (fabs(Vector3D::Dot(normal, arbitraryUp)) > 0.99f) {
		arbitraryUp = Vector3D(1, 0, 0);
	}

	Right = Vector3D::Cross(normal, arbitraryUp).Normalised();

	Up = Vector3D::Cross(Right, normal).Normalised();
}

void Fracturerer::CullPoints(Array<Vector3D>& remainingPoints, Vector3D& CurrentPoint, const Vector3D& normal)
{
	Array<Vector3D> UnculledPoints;

	for (unsigned int i = 1; i < remainingPoints.GetSize(); i++)
	{
		Vector3D dirToPoint = (remainingPoints[i] - CurrentPoint).Normalised();

	//	std::cout << "Dot: " << Vector3D::Dot(normal, dirToPoint) << std::endl;

		if (Vector3D::Dot(normal, dirToPoint) > 0)
		{
			UnculledPoints.Add(remainingPoints[i]);
		}

	}

	std::cout << UnculledPoints.GetSize() << std::endl;

	remainingPoints = UnculledPoints;
}

void Fracturerer::GetClosestPoint(Array<Vector3D>& remainingPoints, Array<Vector3D>& points, Vector3D& CurrentPoint, Vector3D& closestPoint)
{
	for (unsigned int i = 0; i < remainingPoints.GetSize(); i++)
	{

		if (remainingPoints[i] == CurrentPoint)
		{
			continue;
		}

		const float distance = (remainingPoints[i] - CurrentPoint).GetSquaredLength();

		if (distance < (closestPoint - CurrentPoint).GetSquaredLength())
		{
			closestPoint = remainingPoints[i];
		}
	}
}

FractureObject::FractureObject(Array<Face> InFaces, Vector3D Position)
{
	transform = Transform(Position, Vector3D(1, 1, 1), Vector3D::Zero);

	shader = Shader("WireShader", "Contents/Shaders/WireShader/");

	Color = Vector3D::RandomRange(Vector3D::Zero, Vector3D(1, 1, 1));

	AddFaces(InFaces);

	Initialise();

	Renderer::FracturesToDraw.Add(this);

}

void FractureObject::AddFaces(Array<Face>& InFaces)
{
	for (unsigned int i = 0; i < InFaces.GetSize(); i++)
	{
		Face face = InFaces[i];

		for (unsigned int v = 0; v < face.Verticies.GetSize(); i++)
		{
			AddToShape(face.Verticies[i]);
		}
	}
}

void FractureObject::AddToShape(Vertex Vert)
{
	unsigned int index;

	if (verts.Contains(Vert, index))
	{
		Indicies.Add(index);
		return;
	}
	index = verts.GetSize();
	Vert.Color = Color;

	glm::vec4 pos = transform.GetInverseModelMatrix() * glm::vec4(Vert.Position.X, Vert.Position.Y, Vert.Position.Z, 1);
	Vert.Position = Vector3D(pos.x, pos.y, pos.z);

	verts.Add(Vert);
	Indicies.Add(index);
	
}

void FractureObject::Initialise()
{

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	glBufferData(GL_ARRAY_BUFFER, verts.GetSize() * sizeof(Vertex), verts.GetFirstRef(), GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, Indicies.GetSize() * sizeof(unsigned int), Indicies.GetFirstRef(), GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(offsetof(Vertex, Color)));

	glBindVertexArray(0);
}



void FractureObject::Draw()
{
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	shader.Use();

	SetShaderVariables();

	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, Indicies.GetSize(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	glUseProgram(0);

}

void FractureObject::SetShaderVariables()
{
	glm::mat4 model = transform.GetModelMatrix();

	glm::mat4 view = Camera::GetActiveCamera()->GetLook();

	glm::mat4 projection = Camera::GetActiveCamera()->GetProjection();

	SetTransformationVariables(model, view, projection);

}

void FractureObject::SetTransformationVariables(glm::mat4& model, glm::mat4& view, glm::mat4& projection)
{
	shader.SetMatrix4fv("Model", glm::value_ptr(model));

	shader.SetMatrix4fv("View", glm::value_ptr(view));

	shader.SetMatrix4fv("Projection", glm::value_ptr(projection));
}