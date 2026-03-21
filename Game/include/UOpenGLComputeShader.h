


#pragma once
#include <memory>

#include "UBaseComputeShader.h"
#include <string>

#include "CorePaths.h"
#include "glad/glad.h"

namespace OpenGL {

	class UOpenGLComputeShader : public UBaseComputeShader
	{
	public:

		UOpenGLComputeShader() = default;
		~UOpenGLComputeShader() override;
		UOpenGLComputeShader(const std::string_view& InName, const std::string_view& InStorageLocation);

		void Use() override;

		void Dispatch(const size_t& NumGroupsX, const size_t& NumGroupsY, const size_t& NumGroupsZ) override;

		void WaitForCompletion()const override;
		static std::shared_ptr<UBaseComputeShader> CreateOpenGLComputeShader(const std::string_view& InName, const std::string_view& InStorageLocation);

	private:
		size_t ID;
		std::string Name;
		std::string StorageLocation;

		bool DoesComputeShaderExist() const;

		std::string GetShaderLocation() const;

		GLint CompileCompute() const;

		void CreateProgram(const GLuint& Compute);

	};
}
