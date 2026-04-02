// DO NOT MARK except for NumericMin and NumericMax, GetLineIntersectionPointWithPlane, Min, Max, GetSingedDistance, OrderByAngle, GetPlaneAxis, GetPlaneNormal, AnglePointPair
//This is because it has been submitted for my COMP305. Link to Original: https://github.falmouth.ac.uk/GA-Undergrad-Student-Work-25-26/Comp305-Engine-SL295211.git
#pragma once
#include <array>

#include "Array.h"


struct Vector2D;

struct Vector3D
{
public:

	constexpr Vector3D() noexcept : X(0), Y(0), Z(0){};

	Vector3D(const Vector2D& Other);
	static void OrderByAngle(Array<Vector3D>& Vertices, const Vector3D& Center, const Vector3D& Normal);
	static void GetPlaneAxis(const Vector3D& Normal, Vector3D& T, Vector3D& U);
	static Vector3D GetPlaneNormal(const Array<Vector3D>& ClippingPlane, const Vector3D& Center);

	constexpr Vector3D(const double InX, const double InY, const double InZ) noexcept : X(static_cast<float>(InX)), Y(static_cast<float>(InY)), Z(static_cast<float>(InZ)){};

	[[nodiscard]] bool operator==(const Vector3D& Other) const {
		return	IsAlmostEqual(*this, Other);
	}

	[[nodiscard]] Vector3D operator/(const float& Other)const noexcept
	{
		return {X / Other, Y / Other, Z / Other};
	}
	[[nodiscard]] Vector3D operator/(const Vector3D& Other)const
	{
		return {X / Other.X, Y / Other.Y, Z / Other.Z};
	}
	[[nodiscard]] Vector3D operator-() const {
		return {-X, -Y, -Z};
	}
	[[nodiscard]] Vector3D operator-(const Vector3D& Other) const {
		return {X - Other.X, Y - Other.Y, Z - Other.Z};
	}

	[[nodiscard]] Vector3D operator*(const double& Multiplier)const
	{
		return {X * Multiplier, Y * Multiplier, Z * Multiplier};
	}
	[[nodiscard]] Vector3D operator*(const Vector3D& Multiplier)const
	{
		return {X * Multiplier.X, Y * Multiplier.Y, Z * Multiplier.Z};
	}

	[[nodiscard]] Vector3D operator+(const Vector3D& Other)const
	{
		return {X + Other.X, Y + Other.Y, Z + Other.Z};
	}

	[[nodiscard]] Vector3D operator+(const double& Other)const
	{
		return {X + Other, Y + Other, Z + Other};
	}


	void operator+=(const float& Other)
	{
		X += Other;
		Y += Other;
		Z += Other;
	}

	void operator+=(const Vector3D& Other)
	{
		X += Other.X;
		Y += Other.Y;
		Z += Other.Z;
	}

	void operator-=(const float& Other)
	{
		X -= Other;
		Y -= Other;
		Z -= Other;
	}

	void operator-=(const Vector3D& Other)
	{
		X -= Other.X;
		Y -= Other.Y;
		Z -= Other.Z;
	}

	[[nodiscard]] bool operator<(const float& Value) const
	{
		return X < Value && Y < Value && Z < Value;
	}


	[[nodiscard]] bool operator<(const Vector3D& Value) const
	{
		return X < Value.X && Y < Value.Y && Z < Value.Z;
	}


	[[nodiscard]] bool operator>(const float& Value) const
	{
		return X > Value && Y > Value && Z > Value;
	}
	[[nodiscard]] bool operator>(const Vector3D& Value) const
	{
		return X > Value.X && Y > Value.Y && Z > Value.Z;
	}

	[[nodiscard]] bool operator>=(const Vector3D& Value) const
	{
		return X >= Value.X && Y >= Value.Y && Z >= Value.Z;
	}

	[[nodiscard]] bool operator<=(const Vector3D& Value) const
	{
		return X <= Value.X && Y <= Value.Y && Z <= Value.Z;
	}

	[[nodiscard]] bool operator<=(const double& Value) const
	{
		return X <= Value && Y <= Value && Z <= Value;
	}

	[[nodiscard]] Vector3D Normalised() const;

	[[nodiscard]] float GetLength() const;

	[[nodiscard]] float GetSquaredLength() const;

	[[nodiscard]] static Vector3D RandomRange(const Vector3D& Min, const Vector3D& Max);

	[[nodiscard]] static bool WithinRange(const Vector3D& Value, const Vector3D& Min, const Vector3D& Max);


	static void Clamp(Vector3D& Value, const Vector3D& MinRange, const Vector3D& MaxRange);

	[[nodiscard]] static double Dot(const Vector3D& A, const Vector3D& B);

	[[nodiscard]] static Vector3D Cross(const Vector3D& A, const Vector3D& B);

	/**
	 * Gets where a plane and line intersect
	 * @param Mid Center of the plane
	 * @param Normal Planes normal
	 * @param Point Origin of line
	 * @param LineDir Direction of line travel
	 * @param Out The point of intersection
	 * @return Whether the line intersects with the plane
	 */
	static bool GetIntersectionPointWithPlane(const Vector3D& Mid, const Vector3D& Normal, const Vector3D& Point,
	                                          const Vector3D& LineDir, Vector3D& Out);

	static Vector3D GetLineIntersectionPointWithPlane(const Vector3D& Normal, const double& D,
	                                                  const Vector3D& LineStart, const Vector3D& LineEnd);

	[[nodiscard]] static bool IsAlmostEqual(const Vector3D& A, const Vector3D& B, const float& Range = 1e-6f);

	void Print() const;

	[[nodiscard]] static Vector3D Abs(const Vector3D& V);
	[[nodiscard]] Vector3D Abs() const;

	[[nodiscard]] static Vector3D min(const Vector3D& A, const Vector3D B);
	[[nodiscard]] static Vector3D max(const Vector3D& A, const Vector3D B);


	[[nodiscard]] static double GetSignedDistance(const Vector3D& Point, const Vector3D& Normal, const Vector3D& PointOnPlane);

	std::array<double, 3> GetAsDoubleArray() const
	{
		return { X, Y, Z };
	}

	float X = 0;
	float Y = 0;
	float Z = 0;

	static const Vector3D Up;
	static const Vector3D Zero;
	static const Vector3D One;
	static const Vector3D NumericMax;
	static const Vector3D NumericMin;
};



[[nodiscard]] inline Vector3D operator*(const float& Multiplier, const Vector3D& Vec)
{
	return Vec * Multiplier;
}


inline std::ostream& operator<<(std::ostream& os, const Vector3D& V)
{
	os << "{ " << V.X << ", " << V.Y << ", " << V.Z << " }";
	return os;
}
struct AnglePointPair
{
	Vector3D point;
	double angle;

	bool operator<(const AnglePointPair& Other)const
	{
		return angle < Other.angle;
	}

	bool operator==(const AnglePointPair& Other) const
	{
		return point == Other.point;
	}
};
