
#pragma once

#include "ErrorCodes.h"
#include "GraphicsAPIConstructor.h"

namespace Vulkan
{
	class USwapChain;

	class UImageView
	{
	public:

		UImageView(USwapChain* InOwningSwapChain, VkImageViewCreateInfo& CreateInfo);
		~UImageView();

		ErrorCodes Init(VkImageViewCreateInfo& CreateInfo);
		ErrorCodes CreateImageView(VkImageViewCreateInfo& CreateInfo);

		ErrorCodes ShutDown();

	private:

		USwapChain* OwningSwapChain;

		VkImageView ImageView;



	};
}
