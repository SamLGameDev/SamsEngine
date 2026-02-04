// DO NOT MARK.
//This is because it has been submitted for my dissertation. Link to Original: https://github.falmouth.ac.uk/GA-Undergrad-Student-Work-25-26/Dissertation-SL295211.git


#pragma once
#include "BaseRenderer.h"
#include "LinkedList.h"
#include "Camera.h"
#include "FrameBufferFactory.h"
#include "Model.h"
#include "Voronoi.h"
#include "WorldObject.h"

namespace OpenGL {

	class InputManager;
	class Shape;
	class WireObject;
	class Shader;

	class FracturePiece;

	class Renderer final : public BaseRenderer
	{
	public:
		static Array<::FracturePiece3D*> FracturesToDraw3D;
		explicit Renderer();
		~Renderer() override;
		void RenderWorldObjets();

		/**
 * The main render loop, Draws all registered objects and renders them to the screen
 */
		void RenderingLoop();

		void Draw(const size_t& Size) override;

		void ShutDown();

	private:

	};
}
