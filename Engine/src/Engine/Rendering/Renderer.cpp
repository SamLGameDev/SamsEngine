#include "Renderer.h"

#include <iostream>

#include "InputManager.h"
#include "Shader.h"
#include "WireShapes.h"
#include "Voronoi2D.h"

LinkedList<WireObject*> Renderer::WiresToDraw;

LinkedList<Model*> Renderer::ItemsToRender;


Array<FracturePiece*> Renderer::FracturesToDraw;

Renderer::Renderer(InputManager* InInputManager) : WorldObject()
{
	WindowInputManager = InInputManager;
}

Renderer::~Renderer()
{
	//TODO create a manager so the renderer isnt responsible for the destruction of these objects

	//cleanup objects
	/*if (!WiresToDraw.IsEmpty())
	{
		for (WireObject* Wire : WiresToDraw)
		{
			delete Wire;
		}
	}

	if (!FracturesToDraw.IsEmpty())
	{
		for (FractureObject* Fracture : FracturesToDraw)
		{
			delete Fracture;
		}
	}*/

}

void Renderer::AddItemToRender(Model* Item)
{
	unsigned int index;
	if (ItemsToRender.Contains(Item, index))
	{
		std::cout << "AlreadyInRenderer" << "\n";
		return;
	}

	ItemsToRender.Add(Item);
}

void Renderer::CreatePostProcessingScreen()
{
	constexpr float quadVertices[] = {
		// vertex attributes for a quad that fills the entire PostProcessingShader in Normalized Device Coordinates.
		// positions   // texCoords
		-1.0f, 1.0f, 0.0f, 1.0f,

		-1.0f, -1.0f, 0.0f, 0.0f,

		1.0f, -1.0f, 1.0f, 0.0f,

		1.0f, 1.0f, 1.0f, 1.0f
	};

	constexpr unsigned int quadInds[] =
	{
		//triangles, using counter-clockwise winding
		3, 0, 1,
		1, 2, 3
	};



	unsigned int quadVBO, quadEBO;

	glGenVertexArrays(1, &quadVAO);

	glGenBuffers(1, &quadVBO);
	glGenBuffers(1, &quadEBO);

	glBindVertexArray(quadVAO);

	glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, quadEBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(quadInds), &quadInds, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
}

void Renderer::SetUpPostProcessing()
{
	CreatePostProcessingScreen();

	PostProcessingShader = Shader("ScreenShader", "Shaders/");

	PostProcessingShader.Use();
	PostProcessingShader.SetInt("screenTexture", 0);
}

void Renderer::Start()
{
	Object::Start();

	SetUpPostProcessing();

	//Set up the frame buffers, one for post-processing, the other for antialiasing

	auto genTexture = TextureBuffer(GL_TEXTURE_2D, GL_RGB, FrameBufferFactory::GenerateTex2D);

	PostProcessing = FrameBufferFactory::GenerateFrameBuffer(
		&genTexture,
		Vector2D(static_cast<float>(FirstWindow::GetWindowWidth()), static_cast<float>(FirstWindow::GetWindowHeight())));

	genTexture = TextureBuffer(GL_TEXTURE_2D_MULTISAMPLE, GL_RGB, FrameBufferFactory::GenerateTex2DMultiSampled);

	auto genRenderer = RenderBuffer(
		GL_RENDERBUFFER,
		GL_DEPTH24_STENCIL8,
		GL_DEPTH_STENCIL_ATTACHMENT,
		FrameBufferFactory::GenerateRendererMultiSampled);

	AntiAliasing = FrameBufferFactory::GenerateFrameBuffer(
		&genTexture, &genRenderer,
		Vector2D(static_cast<float>(FirstWindow::GetWindowWidth()), static_cast<float>(FirstWindow::GetWindowHeight())));

	glEnable(GL_MULTISAMPLE);


	FractureShader = Shader("Voronoi2D", "Shaders/");
}

void Renderer::Tick(const double& DeltaTime)
{
	Object::Tick(DeltaTime);

	RenderingLoop();
}

void Renderer::DrawWireObjects() const
{
	if (!WiresToDraw.IsEmpty())
	{
		for (unsigned int i = 0; i < WiresToDraw.GetSize(); i++)
		{
			WiresToDraw[i]->Draw();
		}
	}
}

void Renderer::DrawModels() const
{
	if (!ItemsToRender.IsEmpty())
	{
		for (unsigned int i = 0; i < ItemsToRender.GetSize(); i++)
		{
			ItemsToRender[i]->Draw();
		}
	}
}

void Renderer::DrawFractures() const
{
	if (!FracturesToDraw.IsEmpty())
	{
		for (unsigned int i = 0; i < FracturesToDraw.GetSize(); i++)
		{
			//TODO get fracturing working, eventually

			FracturesToDraw[i]->Draw(&FractureShader);

			//FracturesToDraw[i]->Draw();
		}
	}

	//float vertices[] = {
	//	 0.5f,  0.5f, 0.0f,  // top right
	//	 0.5f, -0.5f, 0.0f,  // bottom right
	//	-0.5f, -0.5f, 0.0f,  // bottom left
	//	-0.5f,  0.5f, 0.0f   // top left 
	//};
	//unsigned int indices[] = {  // note that we start from 0!
	//	0, 1, 3,   // first triangle
	//	1, 2, 3    // second triangle
	//};

	//GLuint vao, vbo, ebo;

	//glGenVertexArrays(1, &vao);
	//glGenBuffers(1, &vbo);
	//glGenBuffers(1, &ebo);

	//glBindVertexArray(vao);

	//glBindBuffer(GL_ARRAY_BUFFER, vbo);

	//glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), &vertices, GL_STATIC_DRAW);

	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);

	//glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), &indices, GL_STATIC_DRAW);

	//glEnableVertexAttribArray(0);
	//glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

	//glBindBuffer(GL_ARRAY_BUFFER, 0);
	////glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	//FractureShader.Use();

	//glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

}

void Renderer::RenderWorldObjets() const
{

	glBindFramebuffer(GL_FRAMEBUFFER, AntiAliasing.FBO);

	glEnable(GL_DEPTH_TEST);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	//glEnable(GL_BLEND);
	//glEnable(GL_PROGRAM_POINT_SIZE);
	////glEnable(GL_CULL_FACE);

	//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	DrawWireObjects();

	//DrawFractures();

	DrawModels();

}

void Renderer::DrawSceneTexture() const
{
	//copy the contents of antialiasing over to the post-processing color buffer,
	//so we can apply post-processing to our anti aliased scene

	const FirstWindow* window =  Camera::GetActiveWindow();

	glBindFramebuffer(GL_READ_FRAMEBUFFER, AntiAliasing.FBO);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, PostProcessing.FBO);
	glBlitFramebuffer(
		0,
		0,
		FirstWindow::GetWindowWidth(),
		FirstWindow::GetWindowHeight(),
		0,
		0,
		FirstWindow::GetWindowWidth(),
		FirstWindow::GetWindowHeight(),
		GL_COLOR_BUFFER_BIT,
		GL_NEAREST
		);
}

void Renderer::RenderPostProcessing() const
{
	glDisable(GL_DEPTH_TEST);

	DrawSceneTexture();

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glClear(GL_COLOR_BUFFER_BIT);

	PostProcessingShader.Use();

	glBindVertexArray(quadVAO);
	glBindTexture(GL_TEXTURE_2D, PostProcessing.Texture);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

void Renderer::RenderingLoop() const
{
	RenderWorldObjets();

	RenderPostProcessing();

	WindowInputManager->ProcessInput(Camera::GetActiveWindow()->GetWindow());

	glfwSwapBuffers(Camera::GetActiveWindow()->GetWindow());
	glfwPollEvents();
}
