#pragma once
#include "Array.h"

class FracturePiece2D;
class Model;
class WireObject;
class FracturePiece3D;

class BaseRenderer
{
public:

	BaseRenderer() = default;

	virtual ~BaseRenderer() = default;

	void AddFracture(FracturePiece3D* Piece);
	void AddFracture(FracturePiece2D* Piece);
	void ReplaceFracture(const FracturePiece3D* Old, const FracturePiece3D* New);
	void ReplaceFracture(const FracturePiece2D* Old, const FracturePiece2D* New);
	void AddWireShape(WireObject* Shape);
	void AddModel(Model* InModel);

	virtual void Draw(const size_t& Size) = 0;

	Array<FracturePiece3D*> FracturesToRender;

	Array<WireObject*> WireShapesToRender;

	Array<Model*> ModelsToRender;

	Array <FracturePiece2D*> Fracture2DToRender;

};
