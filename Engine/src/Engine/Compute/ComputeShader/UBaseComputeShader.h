#pragma once


class UBaseComputeShader
{
public:

	virtual ~UBaseComputeShader() = default;

	virtual void Use() = 0;

	virtual void Dispatch(const size_t& NumGroupsX, const size_t& NumGroupsY, const size_t& NumGroupsZ) const = 0;

	virtual void WaitForCompletion() const = 0;

};
