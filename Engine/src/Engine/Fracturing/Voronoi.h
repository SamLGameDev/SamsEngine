
#pragma once

#include "InputAction.h"
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

	FracturePiece3D(Array<Face> cell, Vector3D Point);

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

	void Seperate();

	void Converge();

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

	std::unique_ptr<InputAction> LeftArrow;
	std::unique_ptr<InputAction> RightArrow;
};


class Voronoi
{
public:

	//Fracture the model into a voronoi diagram based on random points
	void FracturePlaneRandom(Model& InModel, Array<FracturePiece3D>& OutFractures, const size_t& NumPoints);

private:
	static void GetFirstIntersection(const Vector3D& Normal, const Vector3D& Center, const Face& CurrentFace, Face& NewFace,
	                                 size_t& FirstIntersectionIndex, Vector3D& FirstIntersection);

	size_t GetAllVertsUntilSecondIntersection(const Vector3D& Normal, const Vector3D& Center, const Face& CurrentFace, Face& NewFace,
	                                          const size_t& FirstIntersectionIndex, Vector3D& SecondIntersection);

	void GetFaceReveresed(Face& IntersectFace, const Face& CurrentFace, Face& NewFace, const size_t& FirstIntersectionIndex,
	                      const Vector3D& FirstIntersection, const Vector3D& SecondIntersection, const size_t& SecondIntersectionIndex);

	void SliceFaceByPlane(const Array<Face>& Faces, const Vector3D& Normal, const Vector3D& Center, Array<Face>& NewFaces,
	                      Face& IntersectFace, const size_t& FaceIndex);

	void SliceShapeByPlane(const Array<Vector3D>& Points, const size_t& Index, Vector3D& CurrentPoint, Array<Face>& Faces, Vector3D& Normal,
		Vector3D& Right, Vector3D& Up, Vector3D& Center, const size_t& J);

	static void DefinePlane(Vector3D& normal, Vector3D& CurrentPoint, Vector3D& closestPoint, Vector3D& Right, Vector3D& Up, Vector3D& PlaneCenter);
	static bool IsPointInPolygon(const Vector3D& Point, const Array<Vector3D>& Polygon, const Vector3D& center);
	static bool IsPointTooClose(const Vector3D& Point, const Array<Vector3D>& Points);

	std::vector<std::unique_ptr<WireObject>> TestSquare;

	Array<Face> fractureFaces;

	Array<FracturePiece3D> Fractures;

};

