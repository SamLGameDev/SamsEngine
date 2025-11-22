#include "InterfaceRenderer.h"
#include "Voronoi.h"
#include "BaseRenderer.h"
#include "WireShapes.h"

BaseRenderer* Renderer::RealRenderer = nullptr;

void Renderer::AddFracture(FracturePiece3D* Piece)
{
	RealRenderer->AddFracture(Piece);
}

void Renderer::AddWireShape(WireObject* Shape)
{
	RealRenderer->AddWireShape(Shape);
}

void Renderer::Draw(const size_t& Size)
{
	RealRenderer->Draw(Size);
}
