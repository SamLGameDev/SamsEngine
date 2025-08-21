#include "Fracturerer.h"
#include "WireShapes.h"
#include "Array.h"
#include "Camera.h"
#include "glm-1.0.1/glm/gtc/type_ptr.hpp"
#include "Renderer.h"
#include "Math.h"

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

			Vector3D normal, Right, Up, PlaneCenter;

			DefinePlane(normal, CurrentPoint, closestPoint, Right, Up, PlaneCenter);
			//(CurrentPoint, CurrentPoint + (normal * 10), Vector3D(0.1, 0.1, 0.6));
		//	std::cout << normal.X << ", " << normal.Y << ", " << normal.Z << std::endl;

			DrawWireCube(PlaneCenter, Vector3D(1, 1, 1), Vector3D(0.1, 0.1, 0.1), Vector3D(0.5, 0.2, 0.2));

			CullPoints(remainingPoints, PlaneCenter, normal);

			Face UnfinishedFace;

			Vector3D first;

			bool bFirst = false;

			Array<Face> CopyFace;

			Array<Vector3D> IntersectionPoints;


			for (unsigned int i = 0; i < CurrentFaces.GetSize(); i++)
			{
				Face face = CurrentFaces[i];
				Array<Vector3D> newVerts;
				for (unsigned int j = 0; j < face.Verticies.GetSize(); j++)
				{
					glm::vec4 ToWorld = glm::vec4(face.Verticies[j].Position.X, face.Verticies[j].Position.Y, face.Verticies[j].Position.Z, 1);

					ToWorld = InModel.BoundingBox->WireTransform.GetModelMatrix() * ToWorld;

					Vector3D current = Vector3D(ToWorld.x, ToWorld.y, ToWorld.z);

					unsigned int nextVer = (j + 1) % face.Verticies.GetSize();

					ToWorld = glm::vec4(face.Verticies[nextVer].Position.X, face.Verticies[nextVer].Position.Y, face.Verticies[nextVer].Position.Z, 1);

					ToWorld = InModel.BoundingBox->WireTransform.GetModelMatrix() * ToWorld;

					Vector3D next = Vector3D(ToWorld.x, ToWorld.y, ToWorld.z);

					Vector3D lineDir = (next - current);

					if (Vector3D::Dot(normal, (current - PlaneCenter)) >= 0)
					{
						unsigned int index;
						if (!newVerts.Contains(current, index))
						{
							newVerts.Add(current);
						}
					}

					Vector3D intersectionPoint;

					if (Vector3D::GetIntersectionPoint(PlaneCenter, normal, current, lineDir, intersectionPoint))
					{
						newVerts.Add(intersectionPoint);
						IntersectionPoints.Add(intersectionPoint);
					}


					if (Vector3D::Dot(normal, (next - PlaneCenter)) >= 0)
					{
						unsigned int index;
						if (!newVerts.Contains(current, index))
						{
							newVerts.Add(current);
						}
					}
				}

				Face newFace;

				for (unsigned int v = 0; v < newVerts.GetSize(); v++)
				{
					Vertex ver;
					ver.Position = newVerts[v];
					newFace.Verticies.Add(ver);
				}

				CopyFace.Add(newFace);

			
			}

			Face inter;

			Vector3D p1 = IntersectionPoints[0];

			Vertex ver;
			ver.Position = p1;

			inter.Verticies.Add(ver);

			Vector3D dir = Right;

			for (unsigned int o =1; o < IntersectionPoints.GetSize(); o++)
			{
				for (unsigned int h = 0; h < IntersectionPoints.GetSize(); h++)
				{
					if (Math::IsNearlyEqual(std::fabs(Vector3D::Dot(dir, (IntersectionPoints[h] - p1).Normalised())), 1))
					{
						ver.Position = IntersectionPoints[h];

						if (dir == Right)
						{
							dir = Up;
							break;
						}
						dir = Up;
						break;

					}
				}
			}

			CopyFace.Add(inter);

	

			CurrentFaces = CopyFace;
		}


		FractureObject* fracture = new FractureObject(CurrentFaces, CurrentPoint);
		//current point defo in world space

		//Vertex point;
		//point.Color = Vector3D::RandomRange(Vector3D::Zero, Vector3D(1,1,1));
		//for (unsigned int j = 0; j < CurrentFaces.GetSize(); j++)
		//{
		//	unsigned int ind = InModel.BoundingBox->Vertices.GetSize();

		//	point.Position = CurrentFaces[j].Verticies[0].Position;

		//	InModel.BoundingBox->Vertices.Add(point);

		//	point.Position = CurrentFaces[j].Verticies[1].Position;

		//	InModel.BoundingBox->Vertices.Add(point);

		//	point.Position = CurrentFaces[j].Verticies[2].Position;

		//	InModel.BoundingBox->Vertices.Add(point);

		//	InModel.BoundingBox->Indices.Add(ind);
		//	InModel.BoundingBox->Indices.Add(ind + 1);
		//	InModel.BoundingBox->Indices.Add(ind + 2);
		//}

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

void Fracturerer::DefinePlane(Vector3D& normal, Vector3D& CurrentPoint, Vector3D& closestPoint, Vector3D& Right, Vector3D& Up, Vector3D& PlaneCenter)
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

		for (unsigned int v = 0; v < face.Verticies.GetSize(); v+=2)
		{
			AddToShape(face.Verticies[v]);
			AddToShape(face.Verticies[(v + 1) % face.Verticies.GetSize()]);
			AddToShape(face.Verticies[(v + 2) % face.Verticies.GetSize()]);
		}
	}
}

void FractureObject::AddToShape(Vertex Vert)
{
	unsigned int index;

	std::cout << "first: " << Vert.first << " Inter: " << Vert.inter << " Last: " << Vert.last << std::endl;

	if (verts.Contains(Vert, index))
	{
		Indicies.Add(index);
		return;
	}
	index = verts.GetSize();
	Vert.Color = Color;

	Vert.Position.Print();

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

	if (Indicies.GetSize() % 3!= 0)
	{
		if (Indicies.GetSize() % 3 == 1)
		{
			std::cout << "NOt Index 1left" << std::endl;
		}
		std::cout << "NOt Index" << std::endl;
	}
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