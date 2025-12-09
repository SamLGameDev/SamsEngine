
#pragma once

#include "InterfaceRenderer.h"
#include "Vector3D.h"
#include "Model.h"
#include "WireShapes.h"
#include "WorldObject.h"


class FracturePiece3D : WorldObject
{
public:

	FracturePiece3D() = default;

	~FracturePiece3D();

	FracturePiece3D(Array<Vector3D> cell, Vector3D Point);

	void Copy(const FracturePiece3D& other)
	{
		color = other.color;

		dir = other.dir;
		transform = other.transform;
		PVAO = other.PVAO;
		PVBO = other.PVBO;
		Verts = other.Verts;
		Inds = other.Inds;
		VAO = other.VAO;
		VBO = other.VBO;
		EBO = other.EBO;
		shader = other.shader;

		::Renderer::ReplaceFracture(&other, this);

		///.Replace(&other,*other.Tick, this,  &Tick);
	}

	FracturePiece3D(const FracturePiece3D& other)
	{
		Copy(other);
	}

	FracturePiece3D& operator=(const FracturePiece3D& other)
	{
		if (this != &other)
		{
			Copy(other);
		}
		return *this;
	}

	FracturePiece3D(FracturePiece3D&& other) noexcept
	{
		Copy(other);
	}

	FracturePiece3D& operator=(FracturePiece3D&& other) noexcept
	{
		if (this != &other)
		{
			Copy(other);
		}
		return *this;
	}

	void Draw();

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


class Voronoi
{
public:
	void FracturePlaneRandom(Model& InModel, Array<FracturePiece3D>& OutFractures);
	static void DefinePlane(Vector3D& normal, Vector3D& CurrentPoint, Vector3D& closestPoint, Vector3D& Right, Vector3D& Up, Vector3D& PlaneCenter);
	bool IsPointInPolygon(Vector3D Point, Array<Vector3D> Polygon, Vector3D center);

private:

	std::vector<std::unique_ptr<WireObject>> TestSquare;

	Array<Face> fractureFaces;


};

