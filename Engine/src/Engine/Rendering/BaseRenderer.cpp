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

void BaseRenderer::RemoveFracture(FracturePiece3D* ToRemove)
{
	FracturesToRender.Remove(ToRemove);
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

void BaseRenderer::RemoveModel(Model* InModel)
{
	ModelsToRender.Remove(InModel);
}

void BaseRenderer::AddMeshFracturePiece(FracturedMeshPiece* Piece)
{
	MeshFracturePiecesToRender.Add(Piece);
}

void BaseRenderer::RemoveMeshFracturePiece(FracturedMeshPiece* Piece)
{
	MeshFracturePiecesToRender.Remove(Piece);
}

void BaseRenderer::ReplaceMeshFracturePiece(const FracturedMeshPiece* Old, const FracturedMeshPiece* New)
{
	FracturedMeshPiece* oldPtr = const_cast<FracturedMeshPiece*>(Old);
	FracturedMeshPiece* newPtr = const_cast<FracturedMeshPiece*>(New);
	MeshFracturePiecesToRender.Replace(oldPtr, newPtr);
}
