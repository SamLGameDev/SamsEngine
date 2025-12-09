// DO NOT MARK.
//This is because it has been submitted for my dissertation. Link to Original: https://github.falmouth.ac.uk/GA-Undergrad-Student-Work-25-26/Dissertation-SL295211.git


#pragma once

#include <string>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "Array.h"
#include "LinkedList.h"
#include "Texture.h"
#include "Vector3D.h"
#include "CubeMap.h"

namespace OpenGL
{

	class Shader
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

		Shader(const Shader& Other)
		{
			StorageLocation = Other.GetRawStorageLocation();
			Name = Other.GetName();
			ID = Other.GetID();
			Textures = Other.Textures;
			Map = Other.Map;
		}

		Shader& operator=(const Shader& Other) {
			if (this != &Other) { // prevent self-assignment
				StorageLocation = Other.StorageLocation;
				Name = Other.Name;
				ID = Other.ID;
				Textures = Other.Textures;
				Map = Other.Map;
			}
			return *this;
		}


		/**
		 * Activates the shader to be applied to draw calls
		 */
		void Use() const;


	
		/**
		 * Apply all the textures for the next set of rendered objects
		 */
		void ApplyTextures() const;

		void AddTexture(const Texture InTexture);
		void AddTexture(const Array<Texture>& InTexture);


		/**
		 * @return The folder containing the shaders
		 */
		[[nodiscard]] std::string GetRawStorageLocation() const
		{
			return StorageLocation;
		}

		[[nodiscard]] std::string GetName() const
		{
			return Name;
		}

		/**
		 * @return The assigned buffer of the shader program
		 */
		[[nodiscard]] unsigned int GetID() const
		{
			return ID;
		}

		[[nodiscard]] Array<Texture> GetTextures() const
		{
			return Textures;
		}

		void AddCubeMap(const CubeMap& InMap);

	private:

		//TODO update all the default files to reflect new system
		/**
		 * Creates a default vertex file for rendering an object
		 * @return true if file successfully created
		 */
		bool CreateDefaultShaderFile() const;

		/**
		 * Creates a default geometry file for rendering an object
		 * @return true if file successfully created
		 */
		bool CreateDefaultGeometryFile() const;

		/**
		 * Creates a default fragment file for rendering an object
		 * @return true if file successfully created
		 */
		bool CreateDefaultFragmentFile() const;


		/**
		 * @return true if the vertex file exists
		 */
		[[nodiscard]] bool DoesVertexShaderExist() const;

		/**
		 * @return true if the geometry file exists
		 */
		[[nodiscard]] bool DoesGeometryShaderExist() const;

		/**
		 * @return true if the fragment file exists
		 */
		[[nodiscard]] bool DoesFragmentShaderExist() const;

		std::string GetPathUntyped() const;


		/**
		 * @return The full path to the vertex shader
		 */
		[[nodiscard]] std::string GetShaderLocation() const;

		/**
		 * @return The full path to the geometry shader
		 */
		[[nodiscard]] std::string GetGeometryLocation() const;

		/**
		 * @return The full path to the fragment shader
		 */
		[[nodiscard]] std::string GetFragmentLocation() const;



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

		[[nodiscard]] std::string ReadFileContents(const std::string_view& Location) const;

		std::string StorageLocation;

		std::string Name;

		unsigned int ID;

		Array<Texture> Textures;

		CubeMap Map;
	};
}
