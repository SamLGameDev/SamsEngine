#include "MathCore.h"
#include "Vector3D.h"

float MathCore::Determinant(const Vector3D& C0, const Vector3D& C1, const Vector3D& C2)
{
	const float d00 = C0.X * (C1.Y * C2.Z - C1.Z * C2.Y);

	const float d01 = C1.X * (C0.Y * C2.Z - C0.Z * C2.Y);

	const float d02 = C2.X * (C0.Y * C1.Z - C0.Z * C1.Y);

	return d00 - d01 + d02;
}

bool MathCore::IsNearlyEqual(const float a, const float b, const float range)
{
	return a - b < range;
}

bool MathCore::IsEven(const int Value)
{
	return Value % 2 == 0;
}