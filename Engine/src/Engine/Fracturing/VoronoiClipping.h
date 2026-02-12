
#pragma once

#include "Voronoi.h"

struct FTetrahedron
{
	Array<Vector3D> Verts;

	Array<Face> Faces;

	FTetrahedron(const Vector3D& A, const Vector3D& B, const Vector3D& C, const Vector3D& D)
	{
		Verts = { A, B, C, D };
		Faces.Add(Face({ A, B, C }));
		Faces.Add({ {A, B, D} });
		Faces.Add({ {A, C, D} });
		Faces.Add({ {B, C, D} });
	}

	FTetrahedron() = default;

};


struct FTriangle
{
	Vector3D Verts[3];

	FTriangle() = default;

	FTriangle(const Vector3D& A, const Vector3D& B, const Vector3D& C)
	{
		Verts[0] = A;
		Verts[1] = B;
		Verts[2] = C;
	}

	Vector3D& operator[](const size_t& Index)
	{
		if (Index > 2)
		{
			throw std::runtime_error("Triangle out of index");
		}

		return Verts[Index];
	}

	const Vector3D& operator[](const size_t& Index) const
	{
		if (Index > 2)
		{
			throw std::runtime_error("Triangle out of index");
		}

		return Verts[Index];
	}

	bool operator==(const FTriangle& Other) const
	{
		const bool bContainsA = Other[0] == Verts[0] || Other[0] == Verts[1] || Other[0] == Verts[2];
		const bool bContainsB = Other[1] == Verts[0] || Other[1] == Verts[1] || Other[1] == Verts[2];
		const bool bContainsC = Other[2] == Verts[0] || Other[2] == Verts[1] || Other[2] == Verts[2];

		if (bContainsA && bContainsB && bContainsC)
		{
			return true;
		}
		return false;
	}

	bool ShareEdge(FTriangle& Other) const
	{
		const bool bContainsA = Other[0] == Verts[0] || Other[0] == Verts[1] || Other[0] == Verts[2]; 
		const bool bContainsB = Other[1] == Verts[0] || Other[1] == Verts[1] || Other[1] == Verts[2];
		const bool bContainsC = Other[2] == Verts[0] || Other[2] == Verts[1] || Other[2] == Verts[2];

		if ((bContainsA && bContainsB) || (bContainsA && bContainsC) || (bContainsB && bContainsC))
		{
			return true;
		}
		return false;

	}


	Vector3D& begin() { return Verts[0]; }
	Vector3D& end() { return Verts[2]; }
};

class FracturedMeshPiece : WorldObject
{
public:

	FracturedMeshPiece() = default;

	FracturedMeshPiece(const Array<Face>& cell, const Vector3D& Point);
	FracturedMeshPiece(const Array<FTriangle>& cell, const Vector3D& Point);


	void Copy(const FracturedMeshPiece& Other)
	{
		Color = Other.Color;

		dir = Other.dir;
		transform = Other.transform;
		//PVAO = Other.PVAO;
		//PVBO = Other.PVBO;
		Verts = Other.Verts;
	//	CellFaces = Other.CellFaces;
		Inds = Other.Inds;
		VAO = Other.VAO;
		VBO = Other.VBO;
		EBO = Other.EBO;
		shader = Other.shader;
		bHidable = Other.bHidable;

		::Renderer::ReplaceMeshFracturePiece(&Other, this);

		TickDel.BindMember(this, &FracturedMeshPiece::Tick);

		InputManager* inputManager = Camera::GetActiveCamera()->GetActiveInputManager();
		LeftArrow = std::make_unique<InputAction>(GLFW_KEY_LEFT, inputManager, Camera::GetActiveWindow());

		LeftArrow->Actions.BindMember(this, &FracturedMeshPiece::Seperate);

		RightArrow = std::make_unique<InputAction>(GLFW_KEY_RIGHT, inputManager, Camera::GetActiveWindow());

		RightArrow->Actions.BindMember(this, &FracturedMeshPiece::Converge);

		Hide = std::make_unique<InputAction>(GLFW_KEY_J, inputManager, Camera::GetActiveWindow());
		Hide->Actions.BindMember(this, &FracturedMeshPiece::ToggleHidden);

	}

	FracturedMeshPiece(const FracturedMeshPiece& Other) : WorldObject()
	{
		Copy(Other);
	}


	void Move(FracturedMeshPiece& Other)
	{
		Color = Other.Color;

		dir = Other.dir;
		transform = Other.transform;
		//PVAO = Other.PVAO;
	//	PVBO = Other.PVBO;
		Verts = Other.Verts;
		//CellFaces = Other.CellFaces;
		Inds = Other.Inds;
		VAO = Other.VAO;
		VBO = Other.VBO;
		EBO = Other.EBO;
		shader = Other.shader;

		bHidable = Other.bHidable;

		::Renderer::ReplaceMeshFracturePiece(&Other, this);

		InputManager* inputManager = Camera::GetActiveCamera()->GetActiveInputManager();
		LeftArrow = std::make_unique<InputAction>(GLFW_KEY_LEFT, inputManager, Camera::GetActiveWindow());

		LeftArrow->Actions.BindMember(this, &FracturedMeshPiece::Seperate);

		RightArrow = std::make_unique<InputAction>(GLFW_KEY_RIGHT, inputManager, Camera::GetActiveWindow());

		RightArrow->Actions.BindMember(this, &FracturedMeshPiece::Converge);

		Hide = std::make_unique<InputAction>(GLFW_KEY_J, inputManager, Camera::GetActiveWindow());
		Hide->Actions.BindMember(this, &FracturedMeshPiece::ToggleHidden);


		TickDel.Remove(&Other, &FracturedMeshPiece::Tick);
		TickDel.BindMember(this, &FracturedMeshPiece::Tick);
	}

	FracturedMeshPiece& operator=(const FracturedMeshPiece& Other)
	{
		if (this != &Other)
		{
			Copy(Other);
		}
		return *this;
	}

	FracturedMeshPiece(FracturedMeshPiece&& Other) noexcept
	{
		Move(Other);
	}

	FracturedMeshPiece& operator=(FracturedMeshPiece&& other) noexcept
	{
		if (this != &other)
		{
			Move(other);
		}
		return *this;
	}

	void Draw();

	void Start() override;

	void Tick(const double& DeltaTime) override;


	Vector3D Color;

	bool bHidden = false;
	bool bHidable = false;

protected:

	void SetupControls(const Vector3D& point);
	void Seperate();
	void Converge();

	void ToggleHidden();

	void TriangulateCell(const Array<Face>& cell);
	void TriangulateCell(const Array<FTriangle>& cell);
	void AddOrMakeInd(const Vector3D& Vert);
	void BufferData();


	Vector3D dir;

	Transform transform;

	Array<Vector3D> Verts;

	Array<uint16_t> Inds;

	Shader shader;

	std::unique_ptr<InputAction> LeftArrow;
	std::unique_ptr<InputAction> RightArrow;

	std::unique_ptr<InputAction> Hide;

	Vector3D Point;

	GLuint VAO, VBO, EBO;

};

class VoronoiClipping
{
public:
	void ClipMeshToVoronoi(Voronoi& Diagram, const Model& Mesh);

	Array<FTetrahedron> TetrahredraliseMesh(const Model& Mesh);

	Array<FracturedMeshPiece> FracturedPieces;
};


