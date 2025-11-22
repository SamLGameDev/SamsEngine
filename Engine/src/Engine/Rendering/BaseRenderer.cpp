#include "BaseRenderer.h"
#include "WireShapes.h"

void BaseRenderer::AddFracture(FracturePiece3D* Piece)
{
	FracturesToRender.Add(Piece);
}

void BaseRenderer::AddWireShape(WireObject* Shape)
{
	WireShapesToRender.Add(Shape);
}
