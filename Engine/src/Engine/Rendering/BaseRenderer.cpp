#include "BaseRenderer.h"
#include "WireShapes.h"
#include "Model.h"
void BaseRenderer::AddFracture(FracturePiece3D* Piece)
{
	FracturesToRender.Add(Piece);
}

void BaseRenderer::AddFracture(FracturePiece2D* Piece)
{
	Fracture2DToRender.Add(Piece);
}

void BaseRenderer::ReplaceFracture(const FracturePiece3D* Old, const FracturePiece3D* New)
{
	FracturePiece3D* oldPtr = const_cast<FracturePiece3D*>(Old);
	FracturePiece3D* newPtr = const_cast<FracturePiece3D*>(New);
	FracturesToRender.Replace(oldPtr, newPtr);
}

void BaseRenderer::ReplaceFracture(const FracturePiece2D* Old, const FracturePiece2D* New)
{
	FracturePiece2D* oldPtr = const_cast<FracturePiece2D*>(Old);
	FracturePiece2D* newPtr = const_cast<FracturePiece2D*>(New);
	Fracture2DToRender.Replace(oldPtr, newPtr);
}

void BaseRenderer::AddWireShape(WireObject* Shape)
{
	WireShapesToRender.Add(Shape);
}

void BaseRenderer::AddModel(Model* InModel)
{
	ModelsToRender.Add(InModel);
}
