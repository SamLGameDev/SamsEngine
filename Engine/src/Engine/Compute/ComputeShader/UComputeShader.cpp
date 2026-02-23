// DO NOT MARK.
//This is because it has been submitted for my dissertation. Link to Original: https://github.falmouth.ac.uk/GA-Undergrad-Student-Work-25-26/Dissertation-SL295211.git

#include "UComputeShader.h"
#include "UBaseComputeShader.h"

std::function<std::shared_ptr<UBaseComputeShader>(const std::string_view& InName, const std::string_view& InStorageLocation)> UComputeShader::ShaderCreationFunc;

void UComputeShader::Use() const
{
	RealComputeShader->Use();
}

UComputeShader::UComputeShader(const std::string_view& InName, const std::string_view& InStorageLocation)
{
	RealComputeShader = ShaderCreationFunc(InName, InStorageLocation);
}

void UComputeShader::Dispatch(const size_t& NumGroupsX, const size_t& NumGroupsY, const size_t& NumGroupsZ) const
{
	RealComputeShader->Dispatch(NumGroupsX, NumGroupsY, NumGroupsZ);
}

void UComputeShader::WaitForCompletion() const
{
	RealComputeShader->WaitForCompletion();
}
