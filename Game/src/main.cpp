
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

	Voronoi2D vorn;
	vorn.FracturePlaneRandom({-1, 1}, {-1, -1}, 
		{1, 1}, {1, -1}, 100);

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
