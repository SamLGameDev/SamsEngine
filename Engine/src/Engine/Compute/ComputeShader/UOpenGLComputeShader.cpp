// DO NOT MARK CreateDefaultShaderFile, CreateDefaultFragmentFile, CreateDefaultGeometryFile.
//This is because it has been submitted for my dissertation. Link to Original: https://github.falmouth.ac.uk/GA-Undergrad-Student-Work-25-26/Dissertation-SL295211.git

#include "UOpenGLComputeShader.h"

#include <functional>
#include <iostream>

#include "FileSaving.h"
#include "../../../../out/build/windows-debug/vcpkg_installed/vcpkg/blds/glfw3/src/3.4-2448ff4533.clean/include/GLFW/glfw3.h"
#include "glad/glad.h"
namespace OpenGL {
	UOpenGLComputeShader::~UOpenGLComputeShader()
	{
		glDeleteProgram(ID);
	}

	UOpenGLComputeShader::UOpenGLComputeShader(const std::string_view& InName, const std::string_view& InStorageLocation)
	{
		StorageLocation = InStorageLocation;

		Name = InName;

		if (!DoesComputeShaderExist())
		{

			std::cerr << "ERROR::SHADER::VERTEX::SHADER NOT CREATED" << '\n';

			return;
		}

		GLint compute = CompileCompute();
		if (compute == -1)
		{
			return;
		}
		CreateProgram(compute);

	}

	UOpenGLComputeShader::~UOpenGLComputeShader()
	{
		glDeleteProgram(ID);
	}

	void UOpenGLComputeShader::Use()
	{
		glUseProgram(ID);
	}

	void UOpenGLComputeShader::Dispatch(const size_t& NumGroupsX, const size_t& NumGroupsY,
	                                    const size_t& NumGroupsZ) 
	{
		glDispatchCompute(NumGroupsX, NumGroupsY, NumGroupsZ);
		glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
	}

	void UOpenGLComputeShader::WaitForCompletion() const
	{

		GLsync fence = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0);

		glClientWaitSync(fence, GL_SYNC_FLUSH_COMMANDS_BIT, GL_TIMEOUT_IGNORED);

		glDeleteSync(fence);
	}

	std::shared_ptr<UBaseComputeShader> UOpenGLComputeShader::CreateOpenGLComputeShader(const std::string_view& InName,
		const std::string_view& InStorageLocation)
	{
		std::shared_ptr<UOpenGLComputeShader> shader = std::make_shared<UOpenGLComputeShader>(InName, InStorageLocation);

		return shader;
	}

	bool UOpenGLComputeShader::DoesComputeShaderExist() const
	{
		struct stat Buffer;

		const bool bDoesFileExist = stat(GetShaderLocation().data(), &Buffer) == 0;

		return bDoesFileExist;
	}

	std::string UOpenGLComputeShader::GetShaderLocation() const
	{

		return CorePaths::Contents.Path + "/" + StorageLocation + Name + ".comp";

	}

	GLint UOpenGLComputeShader::CompileCompute() const
	{
		int  success = 1;

		const std::string computeCodeString = UFileWriter::ReadFileContents(GetShaderLocation(), std::ios::in);

		const char* computeCode = computeCodeString.c_str();

		GLint compute = glCreateShader(GL_COMPUTE_SHADER);
		glShaderSource(compute, 1, &computeCode, NULL);
		glCompileShader(compute);
		glGetShaderiv(compute, GL_COMPILE_STATUS, &success);

		if (!success)
		{
			char infoLog[512];
			glGetShaderInfoLog(compute, 512, NULL, infoLog);
			std::cout << "ERROR::SHADER::COMPUTE::" << Name << "::COMPILATION_FAILED\n" << infoLog << std::endl;

			return -1;
		}
		return compute;
	}

	void UOpenGLComputeShader::CreateProgram(const GLuint& Compute)
	{
		ID = glCreateProgram();
		glAttachShader(ID, Compute);
		glLinkProgram(ID);

		int success = 1;
		glGetProgramiv(ID, GL_LINK_STATUS, &success);


		if (!success)
		{
			char infoLog[512];
			glGetProgramInfoLog(ID, 512, NULL, infoLog);
			std::cout << "ERROR::SHADER::PROGRAM::COMPILATION_FAILED\n" << infoLog << std::endl;
		}

		glDeleteShader(Compute);
	}
}
