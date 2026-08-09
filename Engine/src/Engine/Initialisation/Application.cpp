#include "Application.h"

#include "RuntimeEngineVulkan.h"
#include "ThreadManager.h"

Application::Application(const EGraphicsAPI API)
{
	UThreadManager* threadManager = UThreadManager::Get();



	Vulkan::RuntimeEngine engine;
	engine.Init();

	Model model = Model("/Models/Asteroid/rock.obj", Shader("BasicTexture", "/Shaders/"));
	//Model model = Model("/Models/Bunny/Bunny.obj", Shader("ColorShape", "/Shaders/"));

	Voronoi vorn;
	//vorn.GenerateNewPointSets(model);
	vorn.FracturePlaneRandomGPU(model, 100, 45);

	model.ModelTransform.Position = { 5, 0, 0 };

	while (!engine.ShouldClose()) engine.Loop();

	Vulkan::RuntimeEngine::WaitForFrameToFinish();

	engine.ShutDown();
}
