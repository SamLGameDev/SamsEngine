#pragma once

#include <cstdint>



struct Vector3D;

struct Vector2D
{
public:

	float X, Y;

	constexpr Vector2D()noexcept : X(0), Y(0) {};

	constexpr Vector2D(const float InX, const float InY)noexcept : X(InX), Y(InY) {};
	constexpr Vector2D(const double InX, const double InY)noexcept : X(InX), Y(InY) {};

	constexpr Vector2D(const Vector3D& Other);


	constexpr Vector2D(const std::uint32_t InX, const std::uint32_t InY)noexcept : X(InX), Y(InY) {};

	constexpr Vector2D(const int InX, const int InY)noexcept : X(InX), Y(InY) {};

	[[nodiscard]] constexpr bool operator==(const Vector2D& other) const {
		return	X == other.X && Y == other.Y;
	}

	[[nodiscard]] Vector2D operator+(const Vector2D& other)const
	{
		return { X + other.X, Y + other.Y };
	}

	[[nodiscard]] Vector2D operator/(const float& other)const
	{
		return { X / other, Y / other};
	}
	[[nodiscard]] Vector2D operator/(const Vector2D& other)const
	{
		return { X / other.X, Y / other.Y};
	}
	[[nodiscard]] Vector2D operator-() const {
		return { -X, -Y};
	}
	[[nodiscard]] Vector2D operator-(const Vector2D& other) const {
		return { X - other.X, Y - other.Y};
	}
	[[nodiscard]] Vector2D operator*(const float& multiplier)const
	{
		return { X * multiplier, Y * multiplier};
	}

	~Vector2D() = default;
	static Vector2D RandomRange(const Vector2D& Min, const Vector2D& Max);
	static void Clamp(Vector2D& Value, const Vector2D& MinRange, const Vector2D& MaxRange);

	static Vector2D PerpendicularBisector(const Vector2D& P1, const Vector2D& P2, float& A, float& B, float& C);
	static float Dot(const Vector2D& A, const Vector2D& B);
	Vector2D Normalised() const;
	float GetLength() const;
	float GetSquaredLength() const;
	void Print() const;
	static float Cross(const Vector2D& a, const Vector2D& b);

	static Vector2D Lerp(const Vector2D& A, const Vector2D& B, const float& T);

	const static Vector2D Zero;
};


[[nodiscard]] inline Vector2D operator*(const float& multiplier, const Vector2D& vec)
{
	return vec * multiplier;
}
