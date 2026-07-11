#include <gtest/gtest.h>
#include "MathCore.h"
#include "Vector3D.h"
#include "Array.h"
#include "BaseDelegate.h"
#include "Vector2D.h"
#include "RuntimeEngine.h"
#include "Voronoi2D.h"
#include "RuntimeEngineVulkan.h"
#include "VoronoiClipping.h"
#include <string>

#include "AABB.h"
#include "FileSaving.h"
#include "PlaneClipping.h"
#include "Vector4D.h"
#include "ComputeShader/UComputeShader.h"
#include "LFQueue.h"
#include "ThreadManager.h"

int main(int argc, char** argv) {
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}



void RunEngine(Vulkan::RuntimeEngine& engine)
{

	Model model = Model("/Models/Asteroid/rock.obj", Shader("BasicTexture", "/Shaders/"));
	//Model model = Model("/Models/Bunny/Bunny.obj", Shader("ColorShape", "/Shaders/"));

	Voronoi vorn;
	//vorn.GenerateNewPointSets(model);
	vorn.FracturePlaneRandomGPU(model, 100, 45);

	model.ModelTransform.Position = { 5, 0, 0 };

	while (!engine.ShouldClose()) engine.Loop();

	Vulkan::RuntimeEngine::WaitForFrameToFinish();
}

void EnginePlane()
{
	Vulkan::RuntimeEngine engine;
	engine.Init();
	RunEngine(engine);

	engine.ShutDown();
}


void RunEngineDelaunay(Vulkan::RuntimeEngine& engine)
{

	Model model = Model("/Models/Asteroid/rock.obj", Shader("ColorShape", "/Shaders/"));
	model.ModelTransform.Position = { 5, 0, 0 };
	Voronoi vorn;
	vorn.FracturePlaneRandomGPU(model);
	//std::cout << "Generated Voronoi Diagram with " << vorn.Fractures.GetSize() << " cells." << std::endl;
	//VoronoiClipping clipper;
	//clipper.ClipMeshToVoronoi(vorn, model);

	Vulkan::RuntimeEngine::WaitForFrameToFinish();
}

void RunEngineOpenGL(OpenGL::RuntimeEngine engine)
{
	Model model = Model("/Models/Asteroid/rock.obj", Shader("BasicTexture", "/Shaders/"));
	//Model model = Model("/Models/Bunny/Bunny.obj", Shader("ColorShape", "/Shaders/"));

	Voronoi vorn;
	//vorn.GenerateNewPointSets(model);
	vorn.FracturePlaneRandomGPU(model,100, 45);

	model.ModelTransform.Position = { 5, 0, 0 };

	while (!engine.ShouldClose()) engine.Loop();
}

void OpenGLTest()
{
	OpenGL::RuntimeEngine engine;
	engine.Init();

	RunEngineOpenGL(engine);

	engine.ShutDown();
}


void EngineDelaunay()
{
	Vulkan::RuntimeEngine engine;
	engine.Init();

	RunEngineDelaunay(engine);

	engine.ShutDown();
}

TEST(Fracturing, Diagram) {
	EnginePlane();

	//EngineDelaunay();
	//OpenGLTest();
}


