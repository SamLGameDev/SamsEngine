
// DO NOT MARK.
//This is because it has been submitted for my dissertation. Link to Original: https://github.falmouth.ac.uk/GA-Undergrad-Student-Work-25-26/Dissertation-SL295211.git

#include "Renderer.h"

#include <iostream>

#include "InputManager.h"
#include "Shader.h"
#include "Voronoi.h"
#include "WireShapes.h"
#include "Voronoi2D.h"
#include "VoronoiClipping.h"

namespace OpenGL
{



	Renderer::Renderer()
	{
		::Renderer::RealRenderer = this;
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

	void Renderer::RenderWorldObjets()
	{

		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		glEnable(GL_DEPTH_TEST);

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

		//glEnable(GL_BLEND);
		//glEnable(GL_PROGRAM_POINT_SIZE);
		////glEnable(GL_CULL_FACE);

		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		for (FracturePiece3D* piece : FracturesToRender)
		{
			piece->Draw();
		}

		for (Model* model : ModelsToRender)
		{
			model->Draw();
		}

		for (FracturePiece2D* piece : Fracture2DToRender)
		{
			piece->Draw();
		}
		for (FracturedMeshPiece* shape : MeshFracturePiecesToRender)
		{
			shape->Draw();
		}

	}


	void Renderer::RenderingLoop()
	{
		RenderWorldObjets();

		//RenderPostProcessing();

		glfwSwapBuffers(Camera::GetActiveWindow()->GetWindow());
		glfwPollEvents();
	}

	void Renderer::Draw(const size_t& Size)
	{
		glDrawElements(GL_TRIANGLES, Size, GL_UNSIGNED_SHORT, nullptr);
	}

	void Renderer::ShutDown()
	{
	}
}
