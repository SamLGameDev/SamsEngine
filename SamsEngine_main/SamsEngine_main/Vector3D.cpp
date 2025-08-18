#include "Vector3D.h"

#include<random>
#include "Math.h"

Vector3D const Vector3D::Up = Vector3D(0, 1, 0);

Vector3D const Vector3D::Zero = Vector3D(0, 0, 0);


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

	if (length < 1e-6)
	{
		return Vector3D(0, 0, 0);
	}

	return Vector3D(X / length, Y / length, Z / length);
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
	return a.X * b.X + a.Y * b.Y + a.Z * b.Z;
}

Vector3D Vector3D::Cross(const Vector3D& a, const Vector3D& b)
{

	float X = a.Y * b.Z - a.Z * b.Y;
	float Y = a.Z * b.X - a.X * b.Z;
	float Z = a.X * b.Y - a.Y * b.X;


	return Vector3D(X, Y, Z);
}

bool Vector3D::GetIntersectionPoint(Vector3D& Mid, Vector3D& Normal, Vector3D& point, Vector3D& LineDir, Vector3D& out)
{

	float d = -Dot(Normal, Mid);

	float dot1 = Dot(Normal, LineDir);
	float dot2 = Dot(Normal, point);

	if (dot1 == 0)
	{
//		std::cout << "failed" << std::endl;
		return false;
	}

	//std::cout << "Point" << std::endl;

	float t = -(dot2 + d) / dot1;

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

void Vector3D::Print()
{
	std::cout << "Vector3D: " << X << ", " << Y << ", " << Z << std::endl;
}

Vector3D::~Vector3D()
{
}