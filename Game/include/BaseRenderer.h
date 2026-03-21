// DO NOT MARK except AddFracture(GracturePeiceGPU*)
//This is because it has been submitted for my COMP305. Link to Original: https://github.falmouth.ac.uk/GA-Undergrad-Student-Work-25-26/Comp305-Engine-SL295211.git
#pragma once
#include "Array.h"

class FracturePieceGPU;
class FracturePiece2D;
class Model;
class WireObject;
class FracturePiece3D;
class FracturedMeshPiece;

class BaseRenderer
{
public:

	BaseRenderer() = default;

	virtual ~BaseRenderer() = default;

	void AddFracture(FracturePiece3D* Piece);
	void AddFracture(FracturePiece2D* Piece);
	void AddFracture(FracturePieceGPU* Piece);
	void ReplaceFracture(const FracturePiece3D* Old, const FracturePiece3D* New);
	void RemoveFracture(FracturePiece3D* ToRemove);
	void ReplaceFracture(const FracturePieceGPU* Old, const FracturePieceGPU* New);
	void RemoveFracture(FracturePieceGPU* ToRemove);

	void ReplaceFracture(const FracturePiece2D* Old, const FracturePiece2D* New);
	void AddWireShape(WireObject* Shape);
	void AddModel(Model* InModel);
	void RemoveModel(Model* InModel);
	virtual void Draw(const size_t& Size) = 0;

	void AddMeshFracturePiece(FracturedMeshPiece* Piece);
	void RemoveMeshFracturePiece(FracturedMeshPiece* Piece);
	void ReplaceMeshFracturePiece(const FracturedMeshPiece* Old, const FracturedMeshPiece* New);

	Array<FracturePiece3D*> FracturesToRender;
	Array<FracturePieceGPU*> GPUFracturesToRender;

	Array<WireObject*> WireShapesToRender;

	Array<Model*> ModelsToRender;

	Array <FracturePiece2D*> Fracture2DToRender;

	Array<FracturedMeshPiece*> MeshFracturePiecesToRender;

};
