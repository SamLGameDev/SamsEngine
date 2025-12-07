#pragma once
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

	class Renderer final : public WorldObject
	{
	public:
		static Array<::FracturePiece3D*> FracturesToDraw3D;
		explicit Renderer(::InputManager* InInputManager);
		~Renderer() override;


		/**
		 * Takes a model, and adds it to be rendered, we use instancing, so we only need to add a model once
		 * @param Item RenderableItem
		 */
		static void AddItemToRender(Model* Item);

		void Start() override;

		static LinkedList<Model*> ItemsToRender;

		static LinkedList<::WireObject*> WiresToDraw;
		static Array<FracturePiece*> FracturesToDraw;


		void Tick(const double& DeltaTime) override;

	private:


		/**
		 * The main render loop, Draws all registered objects and renders them to the screen
		 */
		void RenderingLoop();

		void DrawWireObjects() const;
		void DrawModels() const;
		void DrawFractures();
		void RenderWorldObjets();
		void DrawSceneTexture() const;
		void RenderPostProcessing() const;

		/**
		 * Creates the post-processing quad that covers the screen.
		 * Draw using the quadVAO
		 */
		void CreatePostProcessingScreen();


		/**
		 * Sets up the post-processing, creating both the quad and shader.
		 */
		void SetUpPostProcessing();


		::InputManager* WindowInputManager;

		unsigned int  quadVAO;

		Buffers PostProcessing, AntiAliasing;

		::Shader PostProcessingShader;

		::Shader FractureShader;
		::Shader FractureShader3D;
	};
}
