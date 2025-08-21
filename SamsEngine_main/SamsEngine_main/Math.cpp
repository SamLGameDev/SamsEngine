#include "Math.h"
#include "Vector3D.h"


float Math::Determinant(Vector3D C0, Vector3D C1, Vector3D C2)
{
	float d00 = C0.X * (C1.Y * C2.Z - C1.Z * C2.Y);

	float d01 = C1.X * (C0.Y * C2.Z - C0.Z * C2.Y);

	float d02 = C2.X * (C0.Y * C1.Z - C0.Z * C1.Y);

	return d00 - d01 + d02;
}

bool Math::IsNearlyEqual(float a, float b, float range)
{
	return a - b < range;
}

bool Math::IsEven(int Value)
{
	return Value % 2 == 0;
}
