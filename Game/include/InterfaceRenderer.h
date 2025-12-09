#pragma once

class Model;
class WireObject;
class BaseRenderer;
class FracturePiece3D;

class Renderer
{
public:

	static void AddFracture(FracturePiece3D* Piece);

	static void ReplaceFracture(const FracturePiece3D* Old, const FracturePiece3D* New);

	static void AddWireShape(WireObject* Shape);

	static void AddModel(Model* InModel);

	static void Draw(const size_t& Size);

	static BaseRenderer* RealRenderer;
};
