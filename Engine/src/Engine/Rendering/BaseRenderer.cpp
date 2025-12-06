#include "BaseRenderer.h"
#include "WireShapes.h"
#include "Model.h"
void BaseRenderer::AddFracture(FracturePiece3D* Piece)
{
	FracturesToRender.Add(Piece);
}

void BaseRenderer::AddWireShape(WireObject* Shape)
{
	WireShapesToRender.Add(Shape);
}

void BaseRenderer::AddModel(Model* InModel)
{
	ModelsToRender.Add(InModel);
}
