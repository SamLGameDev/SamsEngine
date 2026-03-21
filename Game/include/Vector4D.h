#pragma once
#include <array>

#include "Array.h"


struct Vector2D;
struct Vector3D;

struct Vector4D
{
public:

	constexpr Vector4D() noexcept : X(0), Y(0), Z(0), W(0){};

	Vector4D(const Vector2D& Other);
	Vector4D(const Vector3D& Other, const float& InW = 0);

	constexpr Vector4D(const double InX, const double InY, const double InZ, const double InW) noexcept : X(InX), Y(InY), Z(InZ), W(InW){};


	[[nodiscard]] bool operator==(const Vector4D& Other) const {
		return	IsAlmostEqual(*this, Other);
	}

	[[nodiscard]] Vector4D operator/(const float& Other)const
	{
		return {X / Other, Y / Other, Z / Other, W/Other};
	}
	[[nodiscard]] Vector4D operator/(const Vector4D& Other)const
	{
		return {X / Other.X, Y / Other.Y, Z / Other.Z, W / Other.W};
	}
	[[nodiscard]] Vector4D operator-() const {
		return {-X, -Y, -Z, -W};
	}
	[[nodiscard]] Vector4D operator-(const Vector4D& Other) const {
		return {X - Other.X, Y - Other.Y, Z - Other.Z, W - Other.W};
	}

	[[nodiscard]] Vector4D operator*(const double& Multiplier)const
	{
		return {X * Multiplier, Y * Multiplier, Z * Multiplier, W * Multiplier};
	}
	[[nodiscard]] Vector4D operator*(const Vector4D& Multiplier)const
	{
		return {X * Multiplier.X, Y * Multiplier.Y, Z * Multiplier.Z, W * Multiplier.W};
	}

	[[nodiscard]] Vector4D operator+(const Vector4D& Other)const
	{
		return {X + Other.X, Y + Other.Y, Z + Other.Z, W + Other.W};
	}

	[[nodiscard]] Vector4D operator+(const float& Other)const
	{
		return {X + Other, Y + Other, Z + Other, W + Other};
	}


	void operator+=(const float& Other)
	{
		X += Other;
		Y += Other;
		Z += Other;
		W += Other;
	}

	void operator+=(const Vector4D& Other)
	{
		X += Other.X;
		Y += Other.Y;
		Z += Other.Z;
		W += Other.W;
	}

	void operator-=(const float& Other)
	{
		X -= Other;
		Y -= Other;
		Z -= Other;
		W -= Other;
	}

	void operator-=(const Vector4D& Other)
	{
		X -= Other.X;
		Y -= Other.Y;
		Z -= Other.Z;
		W -= Other.W;
	}

	[[nodiscard]] bool operator<(const float& Value) const
	{
		return X < Value && Y < Value && Z < Value && W < Value;
	}


	[[nodiscard]] bool operator<(const Vector4D& Value) const
	{
		return X < Value.X && Y < Value.Y && Z < Value.Z && W < Value.W;
	}


	[[nodiscard]] bool operator>(const float& Value) const
	{
		return X > Value && Y > Value && Z > Value && W > Value;
	}
	[[nodiscard]] bool operator>(const Vector4D& Value) const
	{
		return X > Value.X && Y > Value.Y && Z > Value.Z && W > Value.W;
	}

	[[nodiscard]] bool operator>=(const Vector4D& Value) const
	{
		return X >= Value.X && Y >= Value.Y && Z >= Value.Z && W >= Value.W;
	}

	[[nodiscard]] bool operator<=(const Vector4D& Value) const
	{
		return X <= Value.X && Y <= Value.Y && Z <= Value.Z && W <= Value.W;
	}

	[[nodiscard]] Vector4D Normalised() const;

	[[nodiscard]] float GetLength() const;

	[[nodiscard]] float GetSquaredLength() const;

	[[nodiscard]] static Vector4D RandomRange(const Vector4D& Min, const Vector4D& Max);

	[[nodiscard]] static bool WithinRange(const Vector4D& Value, const Vector4D& Min, const Vector4D& Max);


	static void Clamp(Vector4D& Value, const Vector4D& MinRange, const Vector4D& MaxRange);


	[[nodiscard]] static bool IsAlmostEqual(const Vector4D& A, const Vector4D& B, const float& Range = 1e-6);

	void Print() const;

	[[nodiscard]] static Vector4D Abs(const Vector4D& V);
	[[nodiscard]] Vector4D Abs() const;

	[[nodiscard]] static Vector4D min(const Vector4D& A, const Vector4D B);
	[[nodiscard]] static Vector4D max(const Vector4D& A, const Vector4D B);

	std::array<double, 4> GetAsDoubleArray() const
	{
		return { X, Y, Z, W };
	}

	float X = 0;
	float Y = 0;
	float Z = 0;
	float W = 0;

	static const Vector4D Zero;
	static const Vector4D One;
	static const Vector4D NumericMax;
	static const Vector4D NumericMin;
};



[[nodiscard]] inline Vector4D operator*(const float& Multiplier, const Vector4D& Vec)
{
	return Vec * Multiplier;
}
