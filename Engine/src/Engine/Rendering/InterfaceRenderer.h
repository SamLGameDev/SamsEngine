#pragma once

class WireObject;
class BaseRenderer;
class FracturePiece3D;

class Renderer
{
public:

	static void AddFracture(FracturePiece3D* Piece);

	static void AddWireShape(WireObject* Shape);

	static void Draw(const size_t& Size);

	static BaseRenderer* RealRenderer;
};
