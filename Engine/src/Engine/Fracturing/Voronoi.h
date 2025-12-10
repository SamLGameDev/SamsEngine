
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

	void GetFirstIntersection(Vector3D normal, Vector3D center, const Face& currentFace, Face newFace,
		size_t& firstIntersectionIndex, Vector3D& firstIntersection);

	size_t GetAllVertsUntilSecondIntersection(Vector3D normal, Vector3D center, const Face& currentFace, Face newFace,
		size_t firstIntersectionIndex, Vector3D& secondIntersection);

	void GetFaceReveresed(Face intersectFace, const Face& currentFace, Face newFace, size_t firstIntersectionIndex,
		Vector3D firstIntersection, Vector3D secondIntersection, size_t secondIntersectionIndex);

	void SliceFaceByPlane(Array<Face>& Faces, Vector3D& Normal, Vector3D& Center, Array<Face>& newFaces,
		Face& intersectFace, const size_t& FaceIndex);

	void SliceShapeByPlane(const Array<Vector3D>& Points, const size_t& Index, Vector3D& CurrentPoint, Array<Face>& Faces, Vector3D& Normal,
		Vector3D& Right, Vector3D& Up, Vector3D& Center, const size_t& J);

	static void DefinePlane(Vector3D& normal, Vector3D& CurrentPoint, Vector3D& closestPoint, Vector3D& Right, Vector3D& Up, Vector3D& PlaneCenter);
	static bool IsPointInPolygon(Vector3D Point, Array<Vector3D> Polygon, Vector3D center);

	std::vector<std::unique_ptr<WireObject>> TestSquare;

	Array<Face> fractureFaces;


};

