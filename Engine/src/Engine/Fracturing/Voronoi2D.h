
#pragma once

#include "Array.h"
#include "Vector2D.h"
#include "Vector3D.h"
#include "Shader.h"
#include "glad/glad.h"

class Voronoi2D
{
public:
	bool GetIntersection(float a, float b, float c, Vector2D From, Vector2D To, Vector2D& intersection);
	void FracturePlaneRandom(Vector2D TopLeft, Vector2D BottomLeft, Vector2D TopRight, Vector2D BottomRight);

private:

	bool IsPointInPolygon(Vector2D Point, Array<Vector2D> Polygon);


};

class FracturePiece
{
public:

	FracturePiece(Array<Vector2D> cell);

	void Draw(const Shader* InShader);

	Vector3D color;

private:


	Array<float> Verts;

	Array<unsigned int> Inds;

	GLuint VAO, VBO, EBO;
};
