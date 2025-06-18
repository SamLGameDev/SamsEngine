#pragma once
class Vector3D
{
public:

	Vector3D();

	Vector3D(float InX, float InY, float InZ);

	bool operator==(const Vector3D& other) const {
		return	X == other.X && Y == other.Y && Z == other.Z;
	}


	~Vector3D();

	float X;
	float Y;
	float Z;

};

