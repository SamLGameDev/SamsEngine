


#pragma once

#include <string>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "Array.h"
#include "BaseShader.h"
#include "LinkedList.h"
#include "Texture.h"
#include "Vector3D.h"
#include "CubeMap.h"

namespace OpenGL
{

	class Shader : public BaseShader
	{
	public:

		Shader();
		~Shader() {}

		/**
		 * loads the shader at that location. It loads all parts of the shader, so vert, geom, and frag
		 * @param InName The name of the shader to load
		 * @param InStorageLocation starts from where the main file is located. i.e. Contents/Shader/
		 */
		Shader(const std::string_view& InName, const std::string_view& InStorageLocation);
		void ApplyTextures() const;

		Shader(const Shader& Other)
		{
			StorageLocation = Other.GetRawStorageLocation();
			Name = Other.GetName();
			ID = Other.GetID();
			Textures = Other.Textures;
			Map = Other.Map;
			UniformMappedData = Other.UniformMappedData;
			UniformBufferID = Other.UniformBufferID;
		}

		Shader& operator=(const Shader& Other) {
			if (this != &Other) { // prevent self-assignment
				StorageLocation = Other.StorageLocation;
				Name = Other.Name;
				ID = Other.ID;
				Textures = Other.Textures;
				Map = Other.Map;
				UniformMappedData = Other.UniformMappedData;
				UniformBufferID = Other.UniformBufferID;
			}
			return *this;
		}

		Shader(Shader&& Other) noexcept
		{
			StorageLocation = Other.GetRawStorageLocation();
			Name = Other.GetName();
			ID = Other.GetID();
			Textures = Other.Textures;
			Map = Other.Map;
			UniformMappedData = Other.UniformMappedData;
			UniformBufferID = Other.UniformBufferID;
		}

		Shader& operator=(Shader&& Other) noexcept {
			if (this != &Other) { // prevent self-assignment
				StorageLocation = Other.StorageLocation;
				Name = Other.Name;
				ID = Other.ID;
				Textures = Other.Textures;
				Map = Other.Map;
				UniformMappedData = Other.UniformMappedData;
				UniformBufferID = Other.UniformBufferID;
			}
			return *this;
		}



		/**
		 * Activates the shader to be applied to draw calls
		 */
		void Use() override;


		void SetUniformBuffer(const size_t& Location, const void* Data, const size_t& Size) override;

		void AddTexture(const Texture InTexture) override;
		void AddTexture(const Array<Texture>& InTexture) override;


		/**
		 * @return The folder containing the shaders
		 */
		[[nodiscard]] std::string GetRawStorageLocation() const override
		{
			return StorageLocation;
		}

		[[nodiscard]] std::string GetName() const override
		{
			return Name;
		}

		/**
		 * @return The assigned buffer of the shader program
		 */
		[[nodiscard]] unsigned int GetID() const override
		{
			return ID;
		}

		[[nodiscard]] Array<Texture> GetTextures() const override
		{
			return Textures;
		}

		void AddCubeMap(const CubeMap& InMap) override;
		void SetInt(const std::string_view& InName, const int& Value) const;

		static std::shared_ptr<BaseShader> CreateOpenGLShader(const std::string_view& InName, const std::string_view& InStorageLocation);

	private:

		//TODO update all the default files to reflect new system
		/**
		 * Creates a default vertex file for rendering an object
		 * @return true if file successfully created
		 */
		bool CreateDefaultShaderFile() const override;

		/**
		 * Creates a default geometry file for rendering an object
		 * @return true if file successfully created
		 */
		bool CreateDefaultGeometryFile() const override;

		/**
		 * Creates a default fragment file for rendering an object
		 * @return true if file successfully created
		 */
		bool CreateDefaultFragmentFile() const override;


		/**
		 * @return true if the vertex file exists
		 */
		[[nodiscard]] bool DoesVertexShaderExist() const override;

		/**
		 * @return true if the geometry file exists
		 */
		[[nodiscard]] bool DoesGeometryShaderExist() const override;

		/**
		 * @return true if the fragment file exists
		 */
		[[nodiscard]] bool DoesFragmentShaderExist() const override;

		std::string GetPathUntyped() const override;


		/**
		 * @return The full path to the vertex shader
		 */
		[[nodiscard]] std::string GetShaderLocation() const override;

		/**
		 * @return The full path to the geometry shader
		 */
		[[nodiscard]] std::string GetGeometryLocation() const override;

		/**
		 * @return The full path to the fragment shader
		 */
		[[nodiscard]] std::string GetFragmentLocation() const override;

		/**
		 * @return The buffer of the vertex shader, if -1 means it failed
		 */
		[[nodiscard]] unsigned int CompileVertex() const;

		/**
		 * @return The buffer of the Geometry shader, if -1 means it failed
		 */
		[[nodiscard]] unsigned int CompileGeometry() const;

		/**
		 * @return The buffer of the Fragment shader, if -1 means it failed
		 */
		[[nodiscard]] unsigned int CompileFragment() const;

		void CreateProgram(const unsigned int& vertex, const unsigned int& fragment, const unsigned int& geometry);

		[[nodiscard]] std::string ReadFileContents(const std::string_view& Location) const override;

		std::string StorageLocation;

		std::string Name;

		unsigned int ID;

		Array<Texture> Textures;

		CubeMap Map;

		Array<size_t> UniformBufferID;

		Array<void*> UniformMappedData;
	};
}
