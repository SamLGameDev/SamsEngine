#pragma once

struct Vector3D
{
public:

	Vector3D() : X(0), Y(0), Z(0){};

	Vector3D(float InX, float InY, float InZ);

	Vector3D(const Vector3D& CopyVec)
	{
		Copy(CopyVec);
	}

	Vector3D& operator=(const Vector3D& CopyVec)
	{
		if (this != &CopyVec)
		{
			Copy(CopyVec);
		}
		return *this;
	}

	void Copy(const Vector3D& CopyVec)
	{
		X = CopyVec.X;
		Y = CopyVec.Y;
		Z = CopyVec.Z;
	}

	[[nodiscard]] bool operator==(const Vector3D& other) const {
		return	X == other.X && Y == other.Y && Z == other.Z;
	}

	[[nodiscard]] Vector3D operator/(const float& other)const
	{
		return {X / other, Y / other, Z / other};
	}
	[[nodiscard]] Vector3D operator/(const Vector3D& other)const
	{
		return {X / other.X, Y / other.Y, Z / other.Z};
	}
	[[nodiscard]] Vector3D operator-() const {
		return {-X, -Y, -Z};
	}
	[[nodiscard]] Vector3D operator-(const Vector3D& other) const {
		return {X - other.X, Y - other.Y, Z - other.Z};
	}

	[[nodiscard]] Vector3D operator*(const float& multiplier)const
	{
		return {X * multiplier, Y * multiplier, Z * multiplier};
	}
	[[nodiscard]] Vector3D operator*(const Vector3D& multiplier)const
	{
		return {X * multiplier.X, Y * multiplier.Y, Z * multiplier.Z};
	}

	[[nodiscard]] Vector3D operator+(const Vector3D& other)const
	{
		return {X + other.X, Y + other.Y, Z + other.Z};
	}

	[[nodiscard]] Vector3D operator+(const float& other)const
	{
		return {X + other, Y + other, Z + other};
	}


	void operator+=(const float& other)
	{
		X += other;
		Y += other;
		Z += other;
	}

	void operator-=(const float& other)
	{
		X -= other;
		Y -= other;
		Z -= other;
	}

	[[nodiscard]] bool operator<(const float& value) const
	{
		return X < value && Y < value && Z < value;
	}

	[[nodiscard]] Vector3D Normalised() const;

	[[nodiscard]] float GetLength() const;

	[[nodiscard]] float GetSquaredLength() const;

	[[nodiscard]] static Vector3D RandomRange(const Vector3D& min, const Vector3D& max);

	static void Clamp(Vector3D& Value, const Vector3D& MinRange, const Vector3D& MaxRange);

	[[nodiscard]] static float Dot(const Vector3D& a, const Vector3D& b);

	[[nodiscard]] static Vector3D Cross(const Vector3D& a, const Vector3D& b);

	/**
	 * Gets where a plane and line intersect
	 * @param Mid Center of the plane
	 * @param Normal Planes normal
	 * @param Point Origin of line
	 * @param LineDir Direction of line travel
	 * @param out The point of intersection
	 * @return Whether the line intersects with the plane
	 */
	static bool GetIntersectionPointWithPlane(const Vector3D& Mid, const Vector3D& Normal, const Vector3D& Point,
	                                          const Vector3D& LineDir, Vector3D& out);

	[[nodiscard]] static bool IsAlmostEqual(const Vector3D& a, const Vector3D& b, float range = 1e-6);

	void Print() const;

	[[nodiscard]] static Vector3D Abs(const Vector3D& v);
	[[nodiscard]] Vector3D Abs() const;

	~Vector3D() = default;

	float X = 0;
	float Y = 0;
	float Z = 0;

	static const Vector3D Up;
	static const Vector3D Zero;
};

[[nodiscard]] inline Vector3D operator*(const float& multiplier, const Vector3D& vec)
{
	return vec * multiplier;
}