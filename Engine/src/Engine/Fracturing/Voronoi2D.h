// DO NOT MARK 
//This is because it has been submitted for my COMP305. Link to Original: https://github.falmouth.ac.uk/GA-Undergrad-Student-Work-25-26/Comp305-Engine-SL295211.git
#pragma once

#include "Array.h"
#include "InterfaceRenderer.h"
#include "Vector2D.h"
#include "Vector3D.h"
#include "Shader.h"
#include "Transform.h"
#include "glad/glad.h"

class FracturePiece2D
{
public:

	FracturePiece2D() = default;

	FracturePiece2D(const Array<Vector2D>& cell, const Vector2D& Point);


	void Copy(const FracturePiece2D& Other)
	{
		color = Other.color;
		transform = Other.transform;
		PVAO = Other.PVAO;
		PVBO = Other.PVBO;
		Verts = Other.Verts;
		Inds = Other.Inds;
		VAO = Other.VAO;
		VBO = Other.VBO;
		EBO = Other.EBO;
		shader = Other.shader;

		::Renderer::ReplaceFracture(&Other, this);

	}

	FracturePiece2D(const FracturePiece2D& Other)
	{
		Copy(Other);
	}


	void Move(FracturePiece2D& Other)
	{
		color = Other.color;

		transform = Other.transform;
		PVAO = Other.PVAO;
		PVBO = Other.PVBO;
		Verts = Other.Verts;
		Inds = Other.Inds;
		VAO = Other.VAO;
		VBO = Other.VBO;
		EBO = Other.EBO;
		shader = Other.shader;

		::Renderer::ReplaceFracture(&Other, this);
	}

	FracturePiece2D& operator=(const FracturePiece2D& Other)
	{
		if (this != &Other)
		{
			Copy(Other);
		}
		return *this;
	}

	FracturePiece2D(FracturePiece2D&& Other) noexcept
	{
		Move(Other);
	}

	FracturePiece2D& operator=(FracturePiece2D&& other) noexcept
	{
		if (this != &other)
		{
			Move(other);
		}
		return *this;
	}

	void Draw();

	Vector3D color;
	static Shader PointShader;


private:


	GLuint PVAO, PVBO;

	Transform transform;

	Shader shader;

	Array<float> Verts;

	Array<uint16_t> Inds;

	GLuint VAO, VBO, EBO;
};


class Voronoi2D
{
public:
	static bool GetIntersection(const float& A, const float& B, const float& c, const Vector2D& From, const Vector2D& To, Vector2D& Intersection);
	void GetFirstIntersection(const Array<Vector2D>& Cell, const float& A, const float& B, const float& C, Array<Vector2D>& NewCell,
	                          size_t& FirstIntersectionIndex, Vector2D& FirstIntersection);
	void GetSecondIntersection(const Array<Vector2D>& Cell, const float& A, const float& B, const float& C, Array<Vector2D>& NewCell,
	                           const size_t& FirstIntersectionIndex, size_t& SecondIntersectionIndex,
	                           Vector2D& SecondIntersection);
	static void GetReversePolygon(const Array<Vector2D>& Cell, Array<Vector2D>& NewCell, const size_t& FirstIntersectionIndex,
	                              const size_t& SecondIntersectionIndex, const Vector2D& FirstIntersection, const Vector2D& SecondIntersection);
	void FracturePlaneRandom(const Vector2D& TopLeft, const Vector2D& BottomLeft, const Vector2D& TopRight, const Vector2D& BottomRight, const
	                         size_t& NumPoints);

private:
	static bool IsPointInPolygon(const Vector2D& Point, const Array<Vector2D>& Polygon);

	Array<FracturePiece2D> Fractures;
};

