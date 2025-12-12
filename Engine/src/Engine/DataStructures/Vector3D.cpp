
#include "Vector3D.h"


#include<random>
#include<iostream>

Vector3D const Vector3D::Up = Vector3D(0, 1, 0);

Vector3D const Vector3D::Zero = Vector3D(0, 0, 0);

Vector3D const Vector3D::One = Vector3D(1, 1, 1);


/// <summary>
/// Normalises the vector, however if the length is near zero by 1e-6, will return a zero vector
/// </summary>
/// <returns></returns>
Vector3D Vector3D::Normalised() const
{
	const float length = GetLength();

	if (length < 1e-6)
	{
		return Zero;
	}

	return {X / length, Y / length, Z / length};
}

float Vector3D::GetLength() const
{
	return std::sqrt(GetSquaredLength());
}

float Vector3D::GetSquaredLength() const
{
	return X * X + Y * Y + Z * Z;
}

Vector3D Vector3D::RandomRange(const Vector3D& Min, const Vector3D& Max)
{
	std::random_device rd;
	std::mt19937 gen(rd());

	const float minX = std::min(Min.X, Max.X);
	const float maxX = std::max(Min.X, Max.X);

	const float minY = std::min(Min.Y, Max.Y);
	const float maxY = std::max(Min.Y, Max.Y);

	const float minZ = std::min(Min.Z, Max.Z);
	const float maxZ = std::max(Min.Z, Max.Z);

	std::uniform_real_distribution<float> distrX(minX, maxX);
	std::uniform_real_distribution<float> distrY(minY, maxY);
	std::uniform_real_distribution<float> distrZ(minZ, maxZ);

	auto point = Vector3D(distrX(gen), distrY(gen), distrZ(gen));

	Clamp(point, Vector3D(minX, minY, minZ), Vector3D(maxX, maxY, maxZ));

	return point;
}

bool Vector3D::WithinRange(const Vector3D& Value, const Vector3D& Min, const Vector3D& Max)
{
	return Value > Min && Value < Max;
}

void Vector3D::Clamp(Vector3D& Value, const Vector3D& MinRange, const Vector3D& MaxRange)
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
}

float Vector3D::Dot(const Vector3D& A, const Vector3D& B)
{
	return A.X * B.X + A.Y * B.Y + A.Z * B.Z;
}

Vector3D Vector3D::Cross(const Vector3D& A, const Vector3D& B)
{
	const float X = A.Y * B.Z - A.Z * B.Y;
	const float Y = A.Z * B.X - A.X * B.Z;
	const float Z = A.X * B.Y - A.Y * B.X;

	return {X, Y, Z};
}

bool Vector3D::GetIntersectionPointWithPlane(const Vector3D& Mid, const Vector3D& Normal, const Vector3D& Point,
                                    const Vector3D& LineDir, Vector3D& Out)
{
	const float d = -Dot(Normal, Mid);

	const float dot1 = Dot(Normal, LineDir);
	const float dot2 = Dot(Normal, Point);

	if (dot1 == 0)
	{
		return false;
	}

	const float t = -(dot2 + d) / dot1;

	if (t < 0.0f || t > 1.0f) {
		return false;
	}

	Out = Point + (t * LineDir);

	return true;
}

bool Vector3D::IsAlmostEqual(const Vector3D& A, const Vector3D& B, const float& Range)
{
	Vector3D diff = A - B;
	return diff < Range && diff > -Range;
}

void Vector3D::Print() const
{
	std::cout << "Vector3D: " << X << ", " << Y << ", " << Z << std::endl;
}

Vector3D Vector3D::Abs(const Vector3D& V)
{
	return {std::abs(V.X), std::abs(V.Y), std::abs(V.Z)};
}

Vector3D Vector3D::Abs() const
{
	return {std::abs(X), std::abs(Y), std::abs(Z)};
}

