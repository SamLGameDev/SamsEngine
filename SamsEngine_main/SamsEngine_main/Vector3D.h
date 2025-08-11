#pragma once

#include <iostream>

struct Vector3D
{
public:

	Vector3D();

	Vector3D(float InX, float InY, float InZ);

	bool operator==(const Vector3D& other) const {
		return	X == other.X && Y == other.Y && Z == other.Z;
	}

	Vector3D operator/(const int& other)const
	{
		return Vector3D(X / other, Y / other, Z / other);
	}
	Vector3D operator-() const {
		return Vector3D(-X, -Y, -Z);
	}
	Vector3D operator-(const Vector3D& other) const {
		return Vector3D(X - other.X, Y - other.Y, Z - other.Z);
	}

	Vector3D operator*(const float& multiplyer)const
	{
		return Vector3D(X * multiplyer, Y * multiplyer, Z * multiplyer);
	}

	Vector3D operator+(const Vector3D& other)const
	{
		return Vector3D(X + other.X, Y + other.Y, Z + other.Z);
	}

	bool operator<(const float& value)
	{
		return (X < value && Y < value && Z < value);
	}

	Vector3D Normalised();

	const float GetLength() const;

	const float GetSquaredLength() const;

	static Vector3D RandomRange(Vector3D min, Vector3D max);

	static void Clamp(Vector3D& Value, Vector3D MinRange, Vector3D MaxRange);

	static float Dot(Vector3D a, Vector3D b);

	static Vector3D Cross(const Vector3D& a, const Vector3D& b);

	static bool GetIntersectionPoint(Vector3D& a, Vector3D& aDir, Vector3D& b, Vector3D& bDir, Vector3D& out);

	static bool IsAlmostEqual(Vector3D& a, Vector3D& b, float range = 1e-6);

	~Vector3D();

	float X;
	float Y;
	float Z;
	
	static const Vector3D Up;
	static const Vector3D Zero;

};


inline Vector3D operator*(const float& multiplyer, const Vector3D& vec)
{
	return vec * multiplyer;
}