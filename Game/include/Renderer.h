#pragma once
#include "LinkedList.h"
#include "Camera.h"
#include "FrameBufferFactory.h"
#include "Model.h"
#include "WorldObject.h"

class InputManager;
class Shape;
class WireObject;
class Shader;

class Renderer final : public WorldObject
{
public:
	explicit Renderer(InputManager* InInputManager);
	~Renderer() override;


	/**
	 * Takes a model, and adds it to be rendered, we use instancing, so we only need to add a model once
	 * @param Item RenderableItem
	 */
	static void AddItemToRender(Model* Item);

	void Start() override;

	static LinkedList<Model*> ItemsToRender;

	static LinkedList<WireObject*> WiresToDraw;
	//static LinkedList<FractureObject*> FracturesToDraw;


	void Tick(const double& DeltaTime) override;

private:


	/**
	 * The main render loop, Draws all registered objects and renders them to the screen
	 */
	void RenderingLoop() const;

	void DrawWireObjects() const;
	void DrawModels() const;
	void DrawFractures() const;
	void RenderWorldObjets() const;
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


	InputManager* WindowInputManager;

	unsigned int  quadVAO;

	Buffers PostProcessing, AntiAliasing;

	Shader PostProcessingShader;
};
