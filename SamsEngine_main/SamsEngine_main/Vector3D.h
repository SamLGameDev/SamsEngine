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

	Vector3D Normalised();

	const float GetLength() const;

	static Vector3D RandomRange(Vector3D min, Vector3D max);

	static void Clamp(Vector3D& Value, Vector3D MinRange, Vector3D MaxRange);

	static float Dot(Vector3D a, Vector3D b);

	~Vector3D();

	float X;
	float Y;
	float Z;

};


inline Vector3D operator*(const float& multiplyer, const Vector3D& vec)
{
	return vec * multiplyer;
}