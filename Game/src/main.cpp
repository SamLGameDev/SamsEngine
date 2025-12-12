
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
	Model backpack = Model("/Models/BackPack/backpack.obj", Shader("BasicTexture", "/Shaders/"));

	Model asteroid = Model("/Models/Asteroid/rock.obj", Shader("BasicTexture", "/Shaders/"));
	
	backpack.ModelTransform.Position = { 5,0, 0 };
	asteroid.ModelTransform.Position = { 0, 0, -3 };

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
