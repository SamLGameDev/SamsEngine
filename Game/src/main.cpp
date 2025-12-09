
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


int main(int argc, char* argv[]) {



	Vulkan::RuntimeEngine engine;
	engine.Init();


	//Voronoi vorn;

	Model* model = new Model("/Models/Asteroid/rock.obj", Shader("BasicTexture", "/Shaders/"));


	Model* model2 = new Model("/Models/BackPack/backpack.obj", Shader("BasicTexture", "/Shaders/"));

	model2->ModelTransform.Position = { 5,0, 0 };



	//vorn.FracturePlaneRandom(*model2);

	while (!Vulkan::RuntimeEngine::ShouldClose())
	{
		engine.Loop();
	}
	Vulkan::RuntimeEngine::WaitForFrameToFinish();
	delete model;
	delete model2;

	engine.ShutDown();
	

	return EXIT_SUCCESS;
}
