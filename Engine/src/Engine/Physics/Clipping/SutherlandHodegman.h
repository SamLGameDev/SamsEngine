// DO NOT MARK.
//This is because it has been submitted for my dissertation. Link to Original: https://github.falmouth.ac.uk/GA-Undergrad-Student-Work-25-26/Dissertation-SL295211.git


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
