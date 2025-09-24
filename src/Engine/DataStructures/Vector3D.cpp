#include "pch.h"
#include "Vector3D.h"


#include<random>
#include<iostream>

Vector3D const Vector3D::Up = Vector3D(0, 1, 0);

Vector3D const Vector3D::Zero = Vector3D(0, 0, 0);


Vector3D::Vector3D(const float InX, const float InY, const float InZ)
{
	X = InX;
	Y = InY;
	Z = InZ;
}

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

Vector3D Vector3D::RandomRange(const Vector3D& min, const Vector3D& max)
{
	std::random_device rd;
	std::mt19937 gen(rd());

	const float minX = std::min(min.X, max.X);
	const float maxX = std::max(min.X, max.X);

	const float minY = std::min(min.Y, max.Y);
	const float maxY = std::max(min.Y, max.Y);

	const float minZ = std::min(min.Z, max.Z);
	const float maxZ = std::max(min.Z, max.Z);

	std::uniform_real_distribution<float> distrX(minX, maxX);
	std::uniform_real_distribution<float> distrY(minY, maxY);
	std::uniform_real_distribution<float> distrZ(minZ, maxZ);

	auto point = Vector3D(distrX(gen), distrY(gen), distrZ(gen));

	Clamp(point, Vector3D(minX, minY, minZ), Vector3D(maxX, maxY, maxZ));

	return point;
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

float Vector3D::Dot(const Vector3D& a, const Vector3D& b)
{
	return a.X * b.X + a.Y * b.Y + a.Z * b.Z;
}

Vector3D Vector3D::Cross(const Vector3D& a, const Vector3D& b)
{
	const float X = a.Y * b.Z - a.Z * b.Y;
	const float Y = a.Z * b.X - a.X * b.Z;
	const float Z = a.X * b.Y - a.Y * b.X;

	return {X, Y, Z};
}

bool Vector3D::GetIntersectionPointWithPlane(const Vector3D& Mid, const Vector3D& Normal, const Vector3D& point,
                                    const Vector3D& LineDir, Vector3D& out)
{
	const float d = -Dot(Normal, Mid);

	const float dot1 = Dot(Normal, LineDir);
	const float dot2 = Dot(Normal, point);

	if (dot1 == 0)
	{
		return false;
	}

	const float t = -(dot2 + d) / dot1;

	if (t < 0.0f || t > 1.0f) {
		return false;
	}

	out = point + (t * LineDir);

	return true;
}

bool Vector3D::IsAlmostEqual(const Vector3D& a, const Vector3D& b, const float range)
{
	return a - b < range;
}

void Vector3D::Print() const
{
	std::cout << "Vector3D: " << X << ", " << Y << ", " << Z << std::endl;
}

Vector3D Vector3D::Abs(const Vector3D& v)
{
	return {std::abs(v.X), std::abs(v.Y), std::abs(v.Z)};
}

Vector3D Vector3D::Abs() const
{
	return {std::abs(X), std::abs(Y), std::abs(Z)};
}

