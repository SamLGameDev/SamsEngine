#pragma once
class Vector2D
{
public:

	Vector2D();

	Vector2D(float InX, float InY);

	bool operator==(const Vector2D& other) const {
		return	X == other.X && Y == other.Y;
	}


	~Vector2D();

	float X;
	float Y;

};

