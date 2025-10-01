
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


int main(int argc, char* argv[]) {


	RuntimeEngine Engine;
	Engine.Init();


	const PointLight pointLight = PointLight(
		Vector3D(0, 0, 0),
		Vector3D(0.0f, 0.0f, 0.0f),
		Vector3D(0.0f, 0.0f, 0.0f),
		Vector3D(0, 0, 0),
		16,
		1,
		0.09,
		0.032);


	const DirectionalLight dirLight = DirectionalLight(
		Vector3D(0.5f, 0.5f, 0.5f),
		Vector3D(0.2f, 0.2f, 0.2f),
		Vector3D(1, 1, 1),
		16);

	const auto spotLight = SpotLight(
		Vector3D(0.5f, 0.5f, 0.5f),
		Vector3D(1, 1, 1),
		Vector3D(0.5, 0.5, 0.5),
		64,
		glm::cos(glm::radians(12.5f)),
		glm::cos(glm::radians(17.5f)));



	Shader LightShader = Shader("LightShader", "Shaders/");

	Model backpack = Model("/Models/Areoplane/scene.gltf", LightShader);

	Shader skyboxShader = Shader("SkyBoxShader", "Shaders/");
	CubeMap skyBoxTexture = CubeMap("CubeMaps/Space/bkg/blue", "None");

	Model skyBox = Model("Models/SkyBox/SkyBox.obj", skyboxShader);

	MeshObject Object1 = CreateObjectRaw<MeshObject>();
	Object1.SMesh.SetMesh(&backpack);

	MeshObject Object2 = CreateObjectRaw<MeshObject>();
	Object2.SMesh.SetMesh(&skyBox);

	backpack.DrawGroup = GL_LESS;

	//Skybox asset https://opengameart.org/content/space-skyboxes-0

	skyBox.DrawGroup = GL_LEQUAL;

	while (!RuntimeEngine::ShouldClose())
	{
		Engine.Loop();
	}

	Engine.ShutDown();
	

	return EXIT_SUCCESS;
}
