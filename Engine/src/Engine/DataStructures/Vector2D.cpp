
#include "Vector2D.h"

#include <algorithm>
#include <iostream>
#include <random>

const Vector2D Vector2D::Zero{0, 0};

Vector2D Vector2D::RandomRange(const Vector2D& min, const Vector2D& max)
{
	std::random_device rd;
	std::mt19937 gen(rd());

	const float minX = std::min(min.X, max.X);
	const float maxX = std::max(min.X, max.X);

	const float minY = std::min(min.Y, max.Y);
	const float maxY = std::max(min.Y, max.Y);

	std::uniform_real_distribution<float> distrX(minX, maxX);
	std::uniform_real_distribution<float> distrY(minY, maxY);

	auto point = Vector2D(distrX(gen), distrY(gen));

	Clamp(point, Vector2D(minX, minY), Vector2D(maxX, maxY));

	return point;
}

void Vector2D::Clamp(Vector2D& Value, const Vector2D& MinRange, const Vector2D& MaxRange)
{
	if (Value.X < MinRange.X)
	{
		Value.X = MinRange.X;
	}
	else if (Value.X > MaxRange.X)
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
}

Vector2D Vector2D::PerpendicularBisector(const Vector2D& P1, const Vector2D& P2, float& a, float& b,float& c)
{
	Vector2D HalfWay = (P1 + P2) / 2;

	a = P2.X - P1.X;
	b = P2.Y - P1.Y;

	c = -HalfWay.X * a - b * HalfWay.Y;

	return HalfWay;


}

float Vector2D::Dot(const Vector2D& a, const Vector2D& b)
{
	return a.X * b.X + a.Y * b.Y;
}

Vector2D Vector2D::Normalised() const
{
	const float length = GetLength();

	if (length < 1e-6)
	{
		return Zero;
	}

	return { X / length, Y / length };
}

float Vector2D::GetLength() const
{
	return std::sqrt(GetSquaredLength());
}

float Vector2D::GetSquaredLength() const
{
	return X * X + Y * Y;
}

void Vector2D::Print() const
{
	std::cout << "Vector2D: " << X << ", " << Y << "\n";
}

float Vector2D::Cross(const Vector2D& a, const Vector2D& b)
{
	const float cross = a.X * b.Y - a.Y *b.X;

	return cross;
}

Vector2D Vector2D::Lerp(const Vector2D& A, const Vector2D& B, const float& t)
{
	return A + (B - A) * t;
}
