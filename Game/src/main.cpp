
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


	Vulkan::RuntimeEngine Engine;
	Engine.Init();


	//const PointLight pointLight = PointLight(
	//	Vector3D(0, 0, 0),
	//	Vector3D(0.0f, 0.0f, 0.0f),
	//	Vector3D(0.0f, 0.0f, 0.0f),
	//	Vector3D(0, 0, 0),
	//	16,
	//	1,
	//	0.09,
	//	0.032);


	//const DirectionalLight dirLight = DirectionalLight(
	//	Vector3D(0.5f, 0.5f, 0.5f),
	//	Vector3D(0.2f, 0.2f, 0.2f),
	//	Vector3D(1, 1, 1),
	//	16);

	//const auto spotLight = SpotLight(
	//	Vector3D(0.5f, 0.5f, 0.5f),
	//	Vector3D(1, 1, 1),
	//	Vector3D(0.5, 0.5, 0.5),
	//	64,
	//	glm::cos(glm::radians(12.5f)),
	//	glm::cos(glm::radians(17.5f)));



	//Shader LightShader = Shader("LightShader", "Shaders/");

	//Model spider = Model("Models/Spider/obj/Only_Spider_with_Animations_Export.obj", LightShader);

	//Model backpack = Model("Models/BackPack/backpack.obj", LightShader);

	//Shader skyboxShader = Shader("SkyBoxShader", "Shaders/");
	//CubeMap skyBoxTexture = CubeMap("CubeMaps/Space/bkg/blue", "None");

	//Model skyBox = Model("Models/SkyBox/SkyBox.obj", skyboxShader);

	//MeshObject Object1 = CreateObjectRaw<MeshObject>();
	//Object1.SMesh.SetMesh(&spider);
	//Object1.transform.Scale = Vector3D(0.1, 0.1, 0.1);

	//MeshObject Object2 = CreateObjectRaw<MeshObject>();
	//Object2.SMesh.SetMesh(&spider);
	//Object2.transform.Scale = Vector3D(0.2, 0.2, 0.2);
	//Object2.transform.Position = Vector3D(0, 0, -40);
	//Object2.transform.Rotation = Vector3D(0, 180, 0);

	//MeshObject Object4 = CreateObjectRaw<MeshObject>();
	//Object1.SMesh.SetMesh(&backpack);

	//MeshObject Object3 = CreateObjectRaw<MeshObject>();
	//Object3.SMesh.SetMesh(&skyBox);

	//spider.DrawGroup = GL_LESS;

	////Skybox asset https://opengameart.org/content/space-skyboxes-0

	//skyBox.DrawGroup = GL_LEQUAL;

	//Voronoi2D v;
	//v.FracturePlaneRandom(Vector2D(-1, 1), Vector2D(-1, -1), Vector2D(1, 1), Vector2D(1, -1));


	while (!RuntimeEngine::ShouldClose())
	{
		Engine.Loop();
	}

	Engine.ShutDown();
	

	return EXIT_SUCCESS;
}
