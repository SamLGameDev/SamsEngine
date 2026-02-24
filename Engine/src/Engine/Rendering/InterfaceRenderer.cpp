#include "InterfaceRenderer.h"
#include "Voronoi.h"
#include "BaseRenderer.h"
#include "WireShapes.h"
#include "Model.h"

BaseRenderer* Renderer::RealRenderer = nullptr;

void Renderer::AddFracture(FracturePiece3D* Piece)
{
	RealRenderer->AddFracture(Piece);
}

void Renderer::AddFracture(FracturePiece2D* Piece)
{
	RealRenderer->AddFracture(Piece);
}

void Renderer::AddFracture(FracturePieceGPU* Piece)
{
	RealRenderer->AddFracture(Piece);
}

void Renderer::ReplaceFracture(const FracturePiece3D* Old, const FracturePiece3D* New)
{
	RealRenderer->ReplaceFracture(Old, New);
}

void Renderer::RemoveFracture(FracturePiece3D* ToRemove)
{
	RealRenderer->RemoveFracture(ToRemove);
}

void Renderer::ReplaceFracture(const FracturePieceGPU* Old, const FracturePieceGPU* New)
{
	RealRenderer->ReplaceFracture(Old, New);
}

void Renderer::RemoveFracture(FracturePieceGPU* ToRemove)
{
	RealRenderer->RemoveFracture(ToRemove);
}

void Renderer::ReplaceFracture(const FracturePiece2D* Old, const FracturePiece2D* New)
{
	RealRenderer->ReplaceFracture(Old, New);
}

void Renderer::AddWireShape(WireObject* Shape)
{
	RealRenderer->AddWireShape(Shape);
}

void Renderer::AddModel(Model* InModel)
{
	RealRenderer->AddModel(InModel);
}

void Renderer::Draw(const size_t& Size)
{
	RealRenderer->Draw(Size);
}

void Renderer::AddMeshFracturePiece(FracturedMeshPiece* Piece)
{
	RealRenderer->AddMeshFracturePiece(Piece);
}

void Renderer::RemoveMeshFracturePiece(FracturedMeshPiece* Piece)
{
	RealRenderer->RemoveMeshFracturePiece(Piece);
}

void Renderer::ReplaceMeshFracturePiece(const FracturedMeshPiece* Old, const FracturedMeshPiece* New)
{
	RealRenderer->ReplaceMeshFracturePiece(Old, New);
}

void Renderer::RemoveModel(Model* InModel)
{
	RealRenderer->RemoveModel(InModel);
}
