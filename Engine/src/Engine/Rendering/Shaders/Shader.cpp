#pragma once
#include "Shader.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include "CorePaths.h"


std::function<std::shared_ptr<BaseShader>(const std::string_view& InName, const std::string_view& InStorageLocation)> Shader::ShaderCreationFunc;

Shader::Shader()
{
}

Shader::~Shader()
{
}

Shader::Shader(const std::string_view& InName, const std::string_view& InStorageLocation)
{
	RealShader = ShaderCreationFunc(InName, InStorageLocation);
}

void Shader::Use() const
{
	RealShader->Use();
}

void Shader::SetUniformBuffer(const size_t& Location, const void* Data, const size_t& Size)
{
	RealShader->SetUniformBuffer(Location, Data, Size);
}



void Shader::AddTexture(const Texture InTexture)
{
	RealShader->AddTexture(InTexture);
}

void Shader::AddTexture(const Array<Texture>& InTexture)
{
	RealShader->AddTexture(InTexture);
}


void Shader::AddCubeMap(const CubeMap& InMap)
{
	RealShader->AddCubeMap(InMap);
}


std::string Shader::GetPathUntyped() const
{
	return RealShader->GetRawStorageLocation();
}

std::string Shader::GetShaderLocation() const
{
	return GetPathUntyped() + ".vert";
}

std::string Shader::GetGeometryLocation() const
{
	return GetPathUntyped() + ".geom";
}

std::string Shader::GetFragmentLocation() const
{
	return GetPathUntyped() + ".frag";
}
