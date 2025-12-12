
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

	Model model = Model("/Models/BackPack/backpack.obj", Shader("BasicTexture", "/Shaders/"));

	model.ModelTransform.Position = { 5,0, 0 };


	Voronoi vorn;
	vorn.FracturePlaneRandom(model, 100);

	while (!RuntimeEngine::ShouldClose())
	{
		engine.Loop();
	}

	Vulkan::RuntimeEngine::WaitForFrameToFinish();
}

int main(int argc, char* argv[])
{
	Vulkan::RuntimeEngine engine;
	engine.Init();

	RunEngine(engine);

	engine.ShutDown();

	return EXIT_SUCCESS;
}
