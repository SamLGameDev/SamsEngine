#include "BaseRenderer.h"
#include "WireShapes.h"
#include "Model.h"
void BaseRenderer::AddFracture(FracturePiece3D* Piece)
{
	FracturesToRender.Add(Piece);
}

void BaseRenderer::ReplaceFracture(const FracturePiece3D* Old, const FracturePiece3D* New)
{
	FracturePiece3D* oldPtr = const_cast<FracturePiece3D*>(Old);
	FracturePiece3D* newPtr = const_cast<FracturePiece3D*>(New);
	FracturesToRender.Replace(oldPtr, newPtr);
}

void BaseRenderer::AddWireShape(WireObject* Shape)
{
	WireShapesToRender.Add(Shape);
}

void BaseRenderer::AddModel(Model* InModel)
{
	ModelsToRender.Add(InModel);
}
