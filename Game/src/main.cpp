
#include <iostream>
#include <cstdlib>
#include "Model.h"
#include "Shader.h"
#include <RuntimeEngine.h>
#include "MeshObject.h"
#include <ObjectFactory.h>

#include <DirectionalLight.h>
#include <PointLight.h>
#include <SpotLight.h>
#include <Voronoi2D.h>
#include <RuntimeEngineVulkan.h>

void RunEngine(Vulkan::RuntimeEngine engine)
{
	Model model = Model("/Models/Asteroid/rock.obj", Shader("BasicTexture", "/Shaders/"));
	model.ModelTransform.Position = { 5, 0, 0 };
	Voronoi vorn;
	vorn.FracturePositions.Position = { 0, 0, 0 };
	vorn.FracturePlaneRandomGPU(model, 10, 0, true);

	Voronoi vorn100;
	vorn100.FracturePositions.Position = { 10, 0 ,0 };
	vorn100.FracturePlaneRandomGPU(model, 100, 0, true);

	while (!engine.ShouldClose())
	{
		engine.Loop();
	}

	Vulkan::RuntimeEngine::WaitForFrameToFinish();
}

void EnginePlane()
{
	Vulkan::RuntimeEngine engine;
	engine.Init();

	RunEngine(engine);

	engine.ShutDown();
}


int main(int argc, char* argv[])
{
	EnginePlane();

	return EXIT_SUCCESS;
}
