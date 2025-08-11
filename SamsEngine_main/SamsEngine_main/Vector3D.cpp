#include "Vector3D.h"

#include<random>

Vector3D::Vector3D()
{
	X = 0;
	Y = 0;
	Z = 0;
}

Vector3D::Vector3D(float InX, float InY, float InZ)
{
	X = InX;
	Y = InY;
	Z = InZ;
}

Vector3D Vector3D::Normalised()
{
	const float length = GetLength();

	return Vector3D(X /= length, Y /= length, Z /= length);
}

const float Vector3D::GetLength() const
{
	return std::sqrt(X*X + Y*Y + Z*Z);
}

const float Vector3D::GetSquaredLength() const
{
	return X * X + Y * Y + Z * Z;
}

Vector3D Vector3D::RandomRange(Vector3D min, Vector3D max)
{
	std::random_device rd;
	std::mt19937 gen(rd());

	float minX = std::min(min.X, max.X);
	float maxX = std::max(min.X, max.X);

	float minY = std::min(min.Y, max.Y);
	float maxY = std::max(min.Y, max.Y);

	float minZ = std::min(min.Z, max.Z);
	float maxZ = std::max(min.Z, max.Z);

	std::uniform_real_distribution<float> distrX(minX, maxX);
	std::uniform_real_distribution<float> distrY(minY, maxY);
	std::uniform_real_distribution<float> distrZ(minZ, maxZ);

	Vector3D point = Vector3D(distrX(gen), distrY(gen), distrZ(gen));

	Clamp(point, Vector3D(minX, minY, minZ), Vector3D(maxX, maxY, maxZ));


	return point;
}

void Vector3D::Clamp(Vector3D& Value, Vector3D MinRange, Vector3D MaxRange)
{

	if (Value.X < MinRange.X)
	{
		Value.X = MinRange.X;
	}
	else if(Value.X > MaxRange.X )
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

float Vector3D::Dot(Vector3D a, Vector3D b)
{
	return a.X * b.X + a.Y * b.Y, + a.Z * b.Z;
}

Vector3D::~Vector3D()
{
}