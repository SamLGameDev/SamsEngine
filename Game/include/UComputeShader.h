


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

	/// <summary>
	/// Binds the shader so buffers can be bound
	/// </summary>
	void Use() const;

	void Dispatch(const size_t& NumGroupsX, const size_t& NumGroupsY, const size_t& NumGroupsZ);

	void WaitForCompletion()const;

private:
	std::shared_ptr<UBaseComputeShader> RealComputeShader;

};
