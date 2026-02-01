
#pragma once

#include <mutex>

#include "Camera.h"

#include "InputAction.h"
#include "InterfaceRenderer.h"
#include "Vector3D.h"
#include "Model.h"
#include "WireShapes.h"
#include "WorldObject.h"


struct TetRing
{
	Edge edge;
	Array<Tetrahedron> Tets;

	bool operator==(const TetRing& Other) const
	{
		return edge == Other.edge;
	}

};

struct VoronoiFace;
class FracturePiece3D : WorldObject
{
public:

	FracturePiece3D() = default;

	~FracturePiece3D();
	void TriangulateCell(const Array<Face>& cell);
	void TriangulateCell(const Array<VoronoiFace>& cell);


	FracturePiece3D(const Array<Face>& cell, const Vector3D& Point);
	void SetupControls(const Vector3D& point);
	void BufferData();

	FracturePiece3D(const Array<VoronoiFace>& cell, const Vector3D& point);

	void Copy(const FracturePiece3D& Other)
	{
		color = Other.color;

		dir = Other.dir;
		transform = Other.transform;
		PVAO = Other.PVAO;
		PVBO = Other.PVBO;
		Verts = Other.Verts;
		CellFaces = Other.CellFaces;
		Inds = Other.Inds;
		VAO = Other.VAO;
		VBO = Other.VBO;
		EBO = Other.EBO;
		shader = Other.shader;
		Point = Other.Point;
		::Renderer::ReplaceFracture(&Other, this);

		TickDel.BindMember(this, &FracturePiece3D::Tick);

		InputManager* inputManager = Camera::GetActiveCamera()->GetActiveInputManager();
		LeftArrow = std::make_unique<InputAction>(GLFW_KEY_LEFT, inputManager, Camera::GetActiveWindow());

		LeftArrow->Actions.BindMember(this, &FracturePiece3D::Seperate);

		RightArrow = std::make_unique<InputAction>(GLFW_KEY_RIGHT, inputManager, Camera::GetActiveWindow());

		RightArrow->Actions.BindMember(this, &FracturePiece3D::Converge);

		Hide = std::make_unique<InputAction>(GLFW_KEY_H, inputManager, Camera::GetActiveWindow());
		Hide->Actions.BindMember(this, &FracturePiece3D::ToggleRendering);

	}

	FracturePiece3D(const FracturePiece3D& Other) : WorldObject()
	{
		Copy(Other);
	}


	void Move(FracturePiece3D& Other)
	{
		color = Other.color;

		dir = Other.dir;
		transform = Other.transform;
		PVAO = Other.PVAO;
		PVBO = Other.PVBO;
		Verts = Other.Verts;
		CellFaces = Other.CellFaces;
		Inds = Other.Inds;
		VAO = Other.VAO;
		VBO = Other.VBO;
		EBO = Other.EBO;
		shader = Other.shader;
		Point = Other.Point;

		::Renderer::ReplaceFracture(&Other, this);

		InputManager* inputManager = Camera::GetActiveCamera()->GetActiveInputManager();
		LeftArrow = std::make_unique<InputAction>(GLFW_KEY_LEFT, inputManager, Camera::GetActiveWindow());

		LeftArrow->Actions.BindMember(this, &FracturePiece3D::Seperate);

		RightArrow = std::make_unique<InputAction>(GLFW_KEY_RIGHT, inputManager, Camera::GetActiveWindow());

		RightArrow->Actions.BindMember(this, &FracturePiece3D::Converge);

		Hide = std::make_unique<InputAction>(GLFW_KEY_H, inputManager, Camera::GetActiveWindow());
		Hide->Actions.BindMember(this, &FracturePiece3D::ToggleRendering);

		TickDel.Remove(&Other, &FracturePiece3D::Tick);
		TickDel.BindMember(this, &FracturePiece3D::Tick);
	}

	FracturePiece3D& operator=(const FracturePiece3D& Other)
	{
		if (this != &Other)
		{
			Copy(Other);
		}
		return *this;
	}

	FracturePiece3D(FracturePiece3D&& Other) noexcept
	{
		Move(Other);
	}

	FracturePiece3D& operator=(FracturePiece3D&& other) noexcept
	{
		if (this != &other)
		{
			Move(other);
		}
		return *this;
	}

	void Seperate();

	void Converge();

	void ToggleRendering();

	void Draw();

	void Start() override;

	void Tick(const double& DeltaTime) override;

	Vector3D color;
	static Shader PointShader;

	Vector3D dir;

	Transform transform;

	Array<VoronoiFace> CellFaces;

	Array<Vector3D> Verts;

	Array<uint16_t> Inds;

	Vector3D Point;
	bool bIsHidden = true;

private:


	GLuint PVAO, PVBO;







	GLuint VAO, VBO, EBO;

	Shader shader;

	std::unique_ptr<InputAction> LeftArrow;
	std::unique_ptr<InputAction> RightArrow;
	std::unique_ptr<InputAction> Hide;



	void AddOrMakeInd(const Vector3D& Vert);
};


struct AnglePointPair
{
	Vector3D point;
	double angle;

	bool operator<(const AnglePointPair& Other)const
	{
		return angle < Other.angle;
	}  

	bool operator==(const AnglePointPair& Other) const
	{
		return point == Other.point;
	}
};

struct VoronoiFace
{
	Array<AnglePointPair> Vertices;

	bool operator==(const VoronoiFace& Other) const
	{
		return Vertices == Other.Vertices;
	}
};


class Voronoi
{
public:

	//Fracture the model into a voronoi diagram based on random points
	void FracturePlaneRandom(Model& InModel, const size_t& NumPoints);
	Array<Vector3D> GenerateRandomPointsInBounds(Model& InModel, const size_t& NumPoints, Array<Vector3D>& Points);

	void FractureDelaunayRandom(Model& InModel, const size_t& NumPoints);

	std::mutex VoronoiMutex;

	Array<FracturePiece3D> Fractures;

	static void OrderVertices(const VoronoiFace& IntersectFace, const Vector3D& Center, const Vector3D& Normal, VoronoiFace& OrderedFace);
	static void OrderVertices(const Array<Vector3D>& Vertices, const Vector3D& Center, const Vector3D& Normal,
		VoronoiFace& OrderedFace);

private:
	static void GetFirstIntersection(const Vector3D& Normal, const Vector3D& Center, const Face& CurrentFace, Face& NewFace,
	                                 size_t& FirstIntersectionIndex, Vector3D& FirstIntersection);

	static size_t GetAllVertsUntilSecondIntersection(const Vector3D& Normal, const Vector3D& Center, const Face& CurrentFace, Face& NewFace,
	                                          const size_t& FirstIntersectionIndex, Vector3D& SecondIntersection);

	static void GetFaceReveresed(Face& IntersectFace, const Face& CurrentFace, Face& NewFace, const size_t& FirstIntersectionIndex,
	                      const Vector3D& FirstIntersection, const Vector3D& SecondIntersection, const size_t& SecondIntersectionIndex);

	void SliceFaceByPlane(const Array<Face>& Faces, const Vector3D& Normal, const Vector3D& Center, Array<Face>& NewFaces,
	                      Face& IntersectFace, const size_t& FaceIndex);

	void SliceShapeByPlane(const Array<Vector3D>& Points, const size_t& Index, Vector3D& CurrentPoint, Array<Face>& Faces, Vector3D& Normal,
		Vector3D& Right, Vector3D& Up, Vector3D& Center, const size_t& J);
	Vector3D ComputePolygonNormal(const Array<Vector3D>& verts);

	static void DefinePlane(Vector3D& normal, const Vector3D& CurrentPoint, const Vector3D& closestPoint, Vector3D& Right, Vector3D& Up, Vector3D& PlaneCenter);
	static bool IsPointInPolygon(const Vector3D& Normal, const Array<Vector3D>& Polygon, const Vector3D& center);
	static bool IsPointTooClose(const Vector3D& Point, const Array<Vector3D>& Points);

	std::vector<std::unique_ptr<WireObject>> TestSquare;

	Array<Face> fractureFaces;

	std::unique_ptr<InputAction> Next;

	static Vector3D GetCircumCenter(const Vector3D& A, const Vector3D& B, const Vector3D& C, const Vector3D& D);
	static void ClipVertexToPlane(const Vector3D& Normal, const double& D, VoronoiFace& IntersectFace, const AnglePointPair& Vertex,
	                              const AnglePointPair& NextVertex, VoronoiFace& NewFace);
	static void GetFaceAxis(const Vector3D& Normal, Vector3D& T, Vector3D& U);

	static void ClipCellToPlane(Array<VoronoiFace>& Cell, const Face& Plane);

	static void ClipCellToBox(const Model& InModel, Array<VoronoiFace>& Cell);
	static void GetAllIncidentTets(const Array<Tetrahedron>& Tetrahedra, const Vector3D& Point, Array<TetRing>& Rings);
	static void GetCellFace(Array<VoronoiFace>& Faces, const TetRing& Ring);
	static Array<VoronoiFace> GetCell(const Array<Tetrahedron>& tetrahedra, const Vector3D& point);
	void GenerateVoronoiCellDelaunay(const Model& InModel, const Array<Tetrahedron>& Tetrahedra, const Vector3D& Point);

	void NextCell();

	size_t current = 0;

	void GenerateVoronoiCellsDelaunay(const Array<Vector3D>& Points, const Model& InModel);

};

