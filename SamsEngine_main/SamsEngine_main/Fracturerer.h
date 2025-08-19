#pragma once

#include"Model.h"

#include "Transform.h"

class Fracturerer
{

public:

	static void Fracture(Model& InModel, const unsigned int FracturePoints);


private:

	static void GetClosestPoint(Array<Vector3D>& remainingPoints, Array<Vector3D>& points, Vector3D& CurrentPoint, Vector3D& closestPoint);

	static void CullPoints(Array<Vector3D>& remainingPoints, Vector3D& CurrentPoint, const Vector3D& normal);


	static void DefinePlane(Vector3D& normal, Vector3D& CurrentPoint, Vector3D& closestPoint, Vector3D& Right, Vector3D& Up, Vector3D& PlaneCenter);


};


class FractureObject
{
public:

	FractureObject(Array<Face> InFaces, Vector3D Position);

	void AddFaces(Array<Face>& InFaces);

	Transform transform;

	Shader shader;

	Array<Vertex> verts;

	Array<unsigned int> Indicies;

	void Initialise();

	void Draw();

private:

	void AddToShape(Vertex Vert);

	unsigned int VAO, VBO, EBO;

	Vector3D Color;


	void SetShaderVariables();

	void SetTransformationVariables(glm::mat4& model, glm::mat4& view, glm::mat4& projection);

};

