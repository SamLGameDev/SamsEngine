
#include "Vector4D.h"


#include<random>
#include<iostream>

#include "Array.h"
#include "Vector2D.h"
#include "Vector3D.h"

Vector4D const Vector4D::Zero = Vector4D(0, 0, 0, 0);

Vector4D const Vector4D::One = Vector4D(1, 1, 1, 1);

Vector4D const Vector4D::NumericMin = Vector4D(std::numeric_limits<float>::lowest(), std::numeric_limits<float>::lowest(),
	std::numeric_limits<float>::lowest(), std::numeric_limits<float>::lowest());

Vector4D const Vector4D::NumericMax = Vector4D(std::numeric_limits<float>::max(), std::numeric_limits<float>::max(),
	std::numeric_limits<float>::max(), std::numeric_limits<float>::max());


/// <summary>
/// Normalises the vector, however if the length is near zero by 1e-6, will return a zero vector
/// </summary>
/// <returns></returns>
Vector4D Vector4D::Normalised() const
{
	const float length = GetLength();

	if (length < 1e-6)
	{
		return Zero;
	}

	return {X / length, Y / length, Z / length, W / length};
}

float Vector4D::GetLength() const
{
	return std::sqrt(GetSquaredLength());
}

float Vector4D::GetSquaredLength() const
{
	return X * X + Y * Y + Z * Z + W * W;
}

Vector4D Vector4D::RandomRange(const Vector4D& Min, const Vector4D& Max)
{
	std::random_device rd;
	std::mt19937 gen(rd());

	const float minX = std::min(Min.X, Max.X);
	const float maxX = std::max(Min.X, Max.X);

	const float minY = std::min(Min.Y, Max.Y);
	const float maxY = std::max(Min.Y, Max.Y);

	const float minZ = std::min(Min.Z, Max.Z);
	const float maxZ = std::max(Min.Z, Max.Z);

	const float minW = std::min(Min.W, Max.W);
	const float maxW = std::max(Min.W, Max.W);

	std::uniform_real_distribution<float> distrX(minX, maxX);
	std::uniform_real_distribution<float> distrY(minY, maxY);
	std::uniform_real_distribution<float> distrZ(minZ, maxZ);
	std::uniform_real_distribution<float> distrW(minW, maxW);

	auto point = Vector4D(distrX(gen), distrY(gen), distrZ(gen), distrW(gen));

	Clamp(point, Vector4D(minX, minY, minZ, minW), Vector4D(maxX, maxY, maxZ, maxW));

	return point;
}

bool Vector4D::WithinRange(const Vector4D& Value, const Vector4D& Min, const Vector4D& Max)
{
	return Value > Min && Value < Max;
}

void Vector4D::Clamp(Vector4D& Value, const Vector4D& MinRange, const Vector4D& MaxRange)
{
	if (Value.X < MinRange.X)
	{
		Value.X = MinRange.X;
	}
	else if (Value.X > MaxRange.X)
	{
		Value.X = MaxRange.X;
	}

	if (Value.Y < MinRange.Y)
	{
		Value.Y = MinRange.Y;
	}
	else if (Value.Y > MaxRange.Y)
	{
		Value.Y = MaxRange.Y;
	}

	if (Value.Z < MinRange.Z)
	{
		Value.Z = MinRange.Z;
	}
	else if (Value.Z > MaxRange.Z)
	{
		Value.Z = MaxRange.Z;
	}

	if (Value.W < MinRange.W)
	{
		Value.W = MinRange.W;
	}
	else if (Value.W > MaxRange.W)
	{
		Value.W = MaxRange.W;
	}
}

bool Vector4D::IsAlmostEqual(const Vector4D& A, const Vector4D& B, const float& Range)
{
	Vector4D diff = A - B;
	return diff < Range && diff > -Range;
}

void Vector4D::Print() const
{
	std::cout << "Vector3D: " << X << ", " << Y << ", " << Z << ", " << W << std::endl;
}

Vector4D Vector4D::Abs(const Vector4D& V)
{
	return {std::fabs(V.X), std::fabs(V.Y), std::fabs(V.Z), std::fabs(V.W)};
}

Vector4D Vector4D::Abs() const
{
	return {std::fabs(X), std::fabs(Y), std::fabs(Z), std::fabs(W)};
}

Vector4D Vector4D::min(const Vector4D& A, const Vector4D B)
{
	return { std::min(A.X, B.X), std::min(A.Y, B.Y), std::min(A.Z, B.Z), std::min(A.W, B.W) };
}

Vector4D Vector4D::max(const Vector4D& A, const Vector4D B)
{
	return { std::max(A.X, B.X), std::max(A.Y, B.Y), std::max(A.Z, B.Z), std::max(A.W, B.W) };
}

Vector4D::Vector4D(const Vector2D& Other)
{
	X = Other.X;
	Y = Other.Y;
	Z = 0;
	W = 0;
}

Vector4D::Vector4D(const Vector3D& Other, const float& InW)
{
	X = Other.X;
	Y = Other.Y;
	Z = Other.Z;
	W = InW;
}
