
#include <iostream>
#include <cstdlib>
#include "Model.h"
#include "Shader.h"
#include <RuntimeEngine.h>
#include "MeshObject.h"
#include <Voronoi2D.h>
#include <RuntimeEngineVulkan.h>

void RunEngineDisplay(OpenGL::RuntimeEngine engine)
{
	Model model = Model("/Models/Asteroid/rock.obj", Shader("BasicTexture", "/Shaders/"));
	model.ModelTransform.Position = { 5, 0, 0 };
	Voronoi vorn;
	vorn.FracturePositions.Position = { 0, 0, 0 };
	vorn.FracturePlaneRandomGPU(model, 10, 0);


	Array<Voronoi*> vorns;

	for (int32_t i = 2; i < 100; i+=5)
	{
		double pos = i + 10;
		Voronoi* vorn100 = new Voronoi;
		vorn100->FracturePositions.Position = { pos, 0 ,0 };
		vorn100->FracturePlaneRandomGPU(model, i, 0);
		vorns.Add(vorn100);
	}

	while (!engine.ShouldClose())
	{
		engine.Loop();
	}

	for (Voronoi* v : vorns)
	{
		delete v;
	}
}

void RunEngineExperimentVulkan(Vulkan::RuntimeEngine engine)
{
	Model model = Model("/Models/Asteroid/rock.obj", Shader("BasicTexture", "/Shaders/"));
	model.ModelTransform.Position = { 5, 0, 0 };
	Voronoi vorn;
	vorn.FracturePlaneRandomGPU(model);

	engine.WaitForFrameToFinish();
}


void RunEngineExperimentOpenGL(OpenGL::RuntimeEngine engine)
{
	Model model = Model("/Models/Asteroid/rock.obj", Shader("BasicTexture", "/Shaders/"));
	model.ModelTransform.Position = { 5, 0, 0 };
	Voronoi vorn;
	vorn.FracturePlaneRandomGPU(model);
}


void EngineExperiment()
{
	Vulkan::RuntimeEngine engine;
	engine.Init();

	RunEngineExperimentVulkan(engine);

	engine.ShutDown();

	OpenGL::RuntimeEngine engineOpenGL;
	engineOpenGL.Init();

	RunEngineExperimentOpenGL(engineOpenGL);

	engineOpenGL.ShutDown();
}


void EnginePlane()
{
	OpenGL::RuntimeEngine engine;
	engine.Init();

	RunEngineDisplay(engine);

	engine.ShutDown();
}


int main(int argc, char* argv[])
{
	std::cout << "Run experiment or View fracture? Type exp for experiment, frac for fracture" << std::endl;

	std::string input;
	std::cin >> input;

	while (input != "exp" && input != "frac")
	{
		std::cout << "Invalid input, please enter 'exp' or 'frac':" << std::endl;
		std::cin >> input;
	}

	if (input == "exp")
	{
		EngineExperiment();
	}
	else if (input == "frac")
	{
		 EnginePlane();
	}

	return EXIT_SUCCESS;
}
