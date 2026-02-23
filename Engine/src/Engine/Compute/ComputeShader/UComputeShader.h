// DO NOT MARK.
//This is because it has been submitted for my dissertation. Link to Original: https://github.falmouth.ac.uk/GA-Undergrad-Student-Work-25-26/Dissertation-SL295211.git


#pragma once
#include <functional>
#include <memory>
#include <string_view>


class UBaseComputeShader;

class UComputeShader
{
public:
	UComputeShader() = default;

	UComputeShader(const std::string_view& InName, const std::string_view& InStorageLocation);

	static std::function<std::shared_ptr<UBaseComputeShader>(const std::string_view& InName, const std::string_view& InStorageLocation)> ShaderCreationFunc;

	void Use() const;

	void Dispatch(const size_t& NumGroupsX, const size_t& NumGroupsY, const size_t& NumGroupsZ) const;

	void WaitForCompletion()const;

private:
	std::shared_ptr<UBaseComputeShader> RealComputeShader;

};
