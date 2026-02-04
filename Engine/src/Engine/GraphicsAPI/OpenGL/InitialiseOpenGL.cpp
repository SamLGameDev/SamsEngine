#include "InitialiseOpenGL.h"

#include <exception>
#include <iostream>
#include "WorldObject.h"
#include "ObjectFactory.h"
#include "glad/glad.h"
#include "GLFW/glfw3.h"

namespace OpenGL
{
	ErrorCodes CInitialiseOpenGL::Init()
	{
		glfwInit();
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, static_cast<int>(OpenGLVersion));

		//do this to get the first digit
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, static_cast<int>(OpenGLVersion * 10) % 10);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

		return SUCCEEDED;
	}

	ErrorCodes CInitialiseOpenGL::Shutdown()
	{
		return SUCCEEDED;
	}
}
