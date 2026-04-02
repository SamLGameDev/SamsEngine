// DO NOT MARK except FracturePlaneRandom, CreateMeshFractureGPU, FracturePlaneRandomGPU, GenerateRandomPointsInBounds, GenerateNewPointSets, FracturePeiceGPU, RawCell, FixedSizeFace, Cell,
// WorkingBuffer, VOut, VOutRaw TetFace, FTet, InTets, VoronoiSSBOIn, TetrahedraliseMesh, GenerateCellGPU, GenerateClippedCellGPU, CleanUpBuffers, DrawFractures, LoadPoints, SetUpComputeShaders, CopyBoundingBoxToBuffer, CleanupGPUGeneration
//This is because it has been submitted for my COMP305. Link to Original: https://github.falmouth.ac.uk/GA-Undergrad-Student-Work-25-26/Comp305-Engine-SL295211.git
#pragma once

#include <mutex>

#include "Camera.h"

#include "InputAction.h"
#include "InterfaceRenderer.h"
#include "Vector3D.h"
#include "Model.h"
#include "PerformanceRecord.h"
#include "UComputeShader.h"
#include "Vector4D.h"
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

struct alignas(16) RawCell
{
	Vector4D Verts[1000];
	uint32_t Inds[20000];
	uint32_t NumInds;
	uint32_t NumVerts;
	uint32_t Padding[2];

};

struct alignas(16) FixedSizeFace
{
	Vector4D Verts[40];
	uint32_t NumVerts;
};

struct alignas (16) Cell
{
	FixedSizeFace Faces[40];
	uint32_t NumFaces;
	uint32_t padding[3];
};

struct alignas(16) WorkingBuffer
{
	Cell cells[100];
	Cell Tempcells[100];
	FixedSizeFace IntersectFaces[100];
};

struct alignas(16) VOut
{
	uint32_t NumCells;
	uint32_t DebugNum;
	uint32_t _Padding[2];
	Cell CutCells[100];

};

struct alignas(16) VOutRaw
{
	uint32_t NumCells;
	uint32_t DebugNum;
	uint32_t _Padding[2];
	RawCell CutCells[100];

};

class FracturePieceGPU : WorldObject
{
public:
	FracturePieceGPU() = default;

	void AddOrMakeInd(const Vector3D& Vert);
	void TriangulateCell(const Array<Face>& cell);
	FracturePieceGPU(Cell& InVoronoiOut, const Vector3D& InPoint);
	FracturePieceGPU(RawCell& InVoronoiOut, const Vector3D& InPoint);
	~FracturePieceGPU() override;
	void Draw();
	void SetupControls(const Vector3D& point);

	void Start() override;

	void Tick(const double& DeltaTime) override;

	Shader shader;

	size_t NumInds;

	GLuint VAO;

	Transform transform;

	Vector3D color;

	Vector3D dir;

	Vector3D Point;

	Array<Vector3D> Verts;
	Array<uint16_t> Inds;

	Array<Face> CellFaces;

	void Seperate();

	void Converge();
	void Copy(const FracturePieceGPU& Other)
	{
		color = Other.color;

		dir = Other.dir;
		transform = Other.transform;
		VAO = Other.VAO;
		shader = Other.shader;
		Point = Other.Point;
		NumInds = Other.NumInds;
		::Renderer::ReplaceFracture(&Other, this);

		TickDel.BindMember(this, &FracturePieceGPU::Tick);

		InputManager* inputManager = Camera::GetActiveCamera()->GetActiveInputManager();
		LeftArrow = std::make_unique<InputAction>(GLFW_KEY_LEFT, inputManager, Camera::GetActiveWindow());

		LeftArrow->Actions.BindMember(this, &FracturePieceGPU::Seperate);

		RightArrow = std::make_unique<InputAction>(GLFW_KEY_RIGHT, inputManager, Camera::GetActiveWindow());

		RightArrow->Actions.BindMember(this, &FracturePieceGPU::Converge);

		//Hide = std::make_unique<InputAction>(GLFW_KEY_H, inputManager, Camera::GetActiveWindow());
		//Hide->Actions.BindMember(this, &FracturePiece3D::ToggleRendering);

	}

	FracturePieceGPU(const FracturePieceGPU& Other) : WorldObject()
	{
		Copy(Other);
	}


	void Move(FracturePieceGPU& Other)
	{
		color = Other.color;

		dir = Other.dir;
		transform = Other.transform;
		VAO = Other.VAO;
		shader = Other.shader;
		Point = Other.Point;
		NumInds = Other.NumInds;

		::Renderer::ReplaceFracture(&Other, this);

		InputManager* inputManager = Camera::GetActiveCamera()->GetActiveInputManager();
		LeftArrow = std::make_unique<InputAction>(GLFW_KEY_LEFT, inputManager, Camera::GetActiveWindow());

		LeftArrow->Actions.BindMember(this, &FracturePieceGPU::Seperate);

		RightArrow = std::make_unique<InputAction>(GLFW_KEY_RIGHT, inputManager, Camera::GetActiveWindow());

		RightArrow->Actions.BindMember(this, &FracturePieceGPU::Converge);

		//Hide = std::make_unique<InputAction>(GLFW_KEY_H, inputManager, Camera::GetActiveWindow());
		//Hide->Actions.BindMember(this, &FracturePiece3D::ToggleRendering);

		TickDel.Remove(&Other, &FracturePieceGPU::Tick);
		TickDel.BindMember(this, &FracturePieceGPU::Tick);
	}

	FracturePieceGPU& operator=(const FracturePieceGPU& Other)
	{
		if (this != &Other)
		{
			Copy(Other);
		}
		return *this;
	}

	FracturePieceGPU(FracturePieceGPU&& Other) noexcept
	{
		Move(Other);
	}

	FracturePieceGPU& operator=(FracturePieceGPU&& other) noexcept
	{
		if (this != &other)
		{
			Move(other);
		}
		return *this;
	}
private:

	std::unique_ptr<InputAction> LeftArrow;
	std::unique_ptr<InputAction> RightArrow;
};


class FracturePiece3D : WorldObject
{
public:

	FracturePiece3D() = default;

	~FracturePiece3D();
	void TriangulateCell(const Array<Face>& cell);

	FracturePiece3D(const Array<Face>& cell, const Vector3D& CellPoint);
	void SetupControls(const Vector3D& point);
	void BufferData();


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

	Array<Face> CellFaces;

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


struct alignas(16) TetFace
{
	Vector4D Verts[3];
};

struct alignas(16) FTet
{
	TetFace TetFaces[4];
};

struct alignas(16) InTets
{
	uint32_t NumTets;
	FTet Tets[1000];
};

struct VoronoiSSBOIn
{
	Vector4D Points[100];
	uint32_t NumPoints;
	FixedSizeFace BoundingBoxFaces[6];
};

class Voronoi
{
public:

	//Fracture the model into a voronoi diagram based on random points
	void FracturePlaneRandom(Model& InModel, const size_t& NumPoints, const size_t& PointSetIndex);
	void GenerateCellGPU(VoronoiSSBOIn* Buffer, const Array<Vector3D>& Points, const GLuint& VoronoiIn, const GLuint& VoronoiOut,
	                     const GLuint& WBuffer,
	                     double& TimeBeforeComputation, UComputeShader& VoronoiCompute);
	void GenerateClippedCellGPU(const Array<Vector3D>& Points, const InTets& Tets, const GLuint& VoronoiOut, const GLuint& ClippedOutInd,
	                            const GLuint& InTetsInd, const GLuint& WBuffer, double& TimeBeforeComputation, UComputeShader& ClippingCompute);
	void CleanUpBuffers(const GLuint& VoronoiIn, const GLuint& VoronoiOut, const GLuint& ClippedOutInd, const GLuint& InTetsInd, const GLuint&
	                    WBuffer);
	void DrawFractures(const Array<Vector3D>& points, const GLuint& ClippedOutInd);
	void CreateMeshFractureGPU(VoronoiSSBOIn* Buffer, const Array<Vector3D>& Points, const InTets& Tets, const GLuint& VoronoiIn,
	                           const GLuint& VoronoiOut, const GLuint& ClippedOutInd, const GLuint& InTetsInd, const GLuint& WBuffer, UComputeShader&
	                           VoronoiCompute, UComputeShader& ClippingCompute);
	void CreateMeshFractureGPU(VoronoiSSBOIn* Buffer, const Array<Vector3D>& Points,
	                           const InTets& Tets, const GLuint& VoronoiIn, const GLuint& VoronoiOut, const GLuint& ClippedOutInd,
	                           const GLuint& InTetsInd, const GLuint& WBuffer, PointEntry& Entry, UComputeShader& VoronoiCompute, UComputeShader&
	                           ClippingCompute);
	static void TetrahedraliseMesh(const Model& InModel, InTets& Tets);
	void CopyBoundingBoxToBuffer(const Model& InModel, VoronoiSSBOIn* Buffer);
	void SetUpComputeShaders(const InTets& Tets, GLuint& VoronoiIn, GLuint& VoronoiOut, GLuint& ClippedOutInd,
	                         GLuint& InTetsInd, GLuint& WBuffer, UComputeShader& ClippingCompute,
	                         UComputeShader& VoronoiCompute);
	void LoadPoints(VoronoiSSBOIn* InBuffer, Array<Vector3D>& Points, const size_t& NumPoints, const std::string& DataToLoad, const size_t&
	                PointSetIndex);

	//Fracture the model into a voronoi diagram based on random points
	void FracturePlaneRandomGPU(Model& InModel);
	void CleanupGPUGeneration(VoronoiSSBOIn* InBuffer, VOut* VoronoiOutBuffer, VOutRaw* ClippedOut, GLuint& VoronoiIn,
	                          GLuint& VoronoiOut,
	                          GLuint& ClippedOutInd, GLuint& InTetsInd, GLuint& wBuffer);


	//Fracture the model into a voronoi diagram based on random points
	void FracturePlaneRandomGPU(Model& InModel, const size_t& NumPoints, const size_t& PointSetIndex);

	static Array<Vector3D> GenerateRandomPointsInBounds(const Model& InModel, const size_t& NumPoints, Array<Vector3D>& Points);

	void FractureDelaunayRandom(Model& InModel, const size_t& NumPoints);

	std::mutex VoronoiMutex;

	Array<FracturePiece3D> Fractures;

	Array<FracturePieceGPU> GPUFractures;

	Transform FracturePositions;


	static void GenerateNewPointSets(const Model& InModel);

private:
	static void DefinePlane(Vector3D& normal, const Vector3D& CurrentPoint, const Vector3D& closestPoint, Vector3D& Right, Vector3D& Up, Vector3D& PlaneCenter);
	static bool IsPointInPolygon(const Vector3D& Normal, const Array<Vector3D>& Polygon, const Vector3D& center);
	static bool IsPointTooClose(const Vector3D& Point, const Array<Vector3D>& Points);

	std::vector<std::unique_ptr<WireObject>> TestSquare;

	Array<Face> fractureFaces;

	std::unique_ptr<InputAction> Next;

	static Vector3D GetCircumCenter(const Vector3D& A, const Vector3D& B, const Vector3D& C, const Vector3D& D);
	static void ClipVertexToPlane(const Vector3D& Normal, const double& D, Face& IntersectFace, const Vector3D& Vertex,
	                              const Vector3D& NextVertex, Face& NewFace);
	static void GetFaceAxis(const Vector3D& Normal, Vector3D& T, Vector3D& U);

	static void ClipCellToPlane(Array<Face>& Cell, const Face& Plane);

	static void ClipCellToBox(const Model& InModel, Array<Face>& Cell);
	static void GetAllIncidentTets(const Array<Tetrahedron>& Tetrahedra, const Vector3D& Point, Array<TetRing>& Rings);
	static void GetCellFace(Array<Face>& Faces, const TetRing& Ring);
	static Array<Face> GetCell(const Array<Tetrahedron>& tetrahedra, const Vector3D& point);
	void GenerateVoronoiCellDelaunay(const Model& InModel, const Array<Tetrahedron>& Tetrahedra, const Vector3D& Point);

	void NextCell();

	size_t current = 0;

	void GenerateVoronoiCellsDelaunay(const Array<Vector3D>& Points, const Model& InModel);

};

