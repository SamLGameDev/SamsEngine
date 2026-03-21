


#pragma once


struct Face;
struct FTriangle;

class SutherlandHodgeman
{

	SutherlandHodgeman() = default;

public:

	static void Clip3D(const Array<Face>& Bounds, const FTriangle& ToClip, Face& OutClipped, const Vector3D& Center);

	static void Clip3D(const Array<FTriangle>& Bounds, const FTriangle& ToClip, Face& OutClipped, const Vector3D& Center);

	static void Clip3D(const Array<FTriangle>& Bounds, const Array<Face>& ToClip, Array<Face>& OutClipped, const Vector3D& Center);

};
