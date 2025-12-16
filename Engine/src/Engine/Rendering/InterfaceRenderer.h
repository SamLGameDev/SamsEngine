#pragma once

class FracturePiece2D;
class Model;
class WireObject;
class BaseRenderer;
class FracturePiece3D;

class Renderer
{
public:

	static void AddFracture(FracturePiece3D* Piece);
	static void AddFracture(FracturePiece2D* Piece);

	static void ReplaceFracture(const FracturePiece3D* Old, const FracturePiece3D* New);
	static void RemoveFracture(FracturePiece3D* ToRemove);

	static void ReplaceFracture(const FracturePiece2D* Old, const FracturePiece2D* New);


	static void AddWireShape(WireObject* Shape);

	static void AddModel(Model* InModel);
	static void RemoveModel(Model* InModel);

	static void Draw(const size_t& Size);

	static BaseRenderer* RealRenderer;
};

