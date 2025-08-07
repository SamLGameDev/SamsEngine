#pragma once
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


	~Vector3D();

	float X;
	float Y;
	float Z;

};

