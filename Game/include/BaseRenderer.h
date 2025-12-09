#pragma once
#include "Array.h"

class Model;
class WireObject;
class FracturePiece3D;

class BaseRenderer
{
public:

	BaseRenderer() = default;

	virtual ~BaseRenderer() = default;

	void AddFracture(FracturePiece3D* Piece);
	void ReplaceFracture(const FracturePiece3D* Old, const FracturePiece3D* New);
	void AddWireShape(WireObject* Shape);
	void AddModel(Model* InModel);

	virtual void Draw(const size_t& Size) = 0;

	Array<FracturePiece3D*> FracturesToRender;

	Array<WireObject*> WireShapesToRender;

	Array<Model*> ModelsToRender;

};
