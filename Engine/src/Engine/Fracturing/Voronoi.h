
#pragma once

#include "Vector3D.h"
#include "Model.h"
#include "WireShapes.h"
#include "WorldObject.h"

class Voronoi
{
public:

	void FracturePlaneRandom(Model& InModel);
	static void DefinePlane(Vector3D& normal, Vector3D& CurrentPoint, Vector3D& closestPoint, Vector3D& Right, Vector3D& Up, Vector3D& PlaneCenter);
	bool IsPointInPolygon(Vector3D Point, Array<Vector3D> Polygon, Vector3D center);

private:

	std::vector<std::unique_ptr<WireObject>> TestSquare;

	Array<Face> fractureFaces;


};
class FracturePiece3D : WorldObject
{
public:

	FracturePiece3D(Array<Vector3D> cell, Vector3D Point);

	void Draw(Shader* InShader);

	void Start() override;

	void Tick(const double& DeltaTime) override;

	Vector3D color;
	static Shader PointShader;

	Vector3D dir;

	Transform transform;


private:


	GLuint PVAO, PVBO;


	Array<float> Verts;

	Array<uint16_t> Inds;

	GLuint VAO, VBO, EBO;

	Shader shader;
};
