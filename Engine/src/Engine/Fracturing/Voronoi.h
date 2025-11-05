
#pragma once

#include "Vector3D.h"
#include "Model.h"
#include "WireShapes.h"

class Voronoi
{
public:

	void FracturePlaneRandom(Model& InModel);
	static void DefinePlane(Vector3D& normal, Vector3D& CurrentPoint, Vector3D& closestPoint, Vector3D& Right, Vector3D& Up, Vector3D& PlaneCenter);

private:

	std::vector<std::unique_ptr<WireObject>> TestSquare;


};
