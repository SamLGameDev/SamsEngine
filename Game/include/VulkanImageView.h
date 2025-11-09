
#pragma once

#include "ErrorCodes.h"
#include "GraphicsAPIConstructor.h"
#include "VulkanSwapChain.h"

namespace Vulkan
{
	class USwapChain;

	class UImageView
	{
	public:

		UImageView()=default;

		UImageView(USwapChain* InOwningSwapChain, VkImageViewCreateInfo& CreateInfo);
		~UImageView();

		UImageView(const UImageView& Other)
		{
			Copy(Other);
		}

		UImageView& operator=(const UImageView& Other)
		{
			if (this != &Other)
			{
				Copy(Other);
			}
			return *this;
		}

		void Copy(const UImageView& Other)
		{
			if (ImageView != VK_NULL_HANDLE) ShutDown();

			ImageView = Other.ImageView;

			OwningSwapChain = Other.OwningSwapChain;
		}

		UImageView(UImageView&& Other) noexcept
		{
			Move(Other);
		}

		UImageView& operator=(UImageView&& Other) noexcept
		{
			if (this != &Other)
			{
				Move(Other);
			}
			return *this;
		}

		void Move(UImageView& Other)
		{
			if (ImageView != VK_NULL_HANDLE) ShutDown();

			ImageView = Other.ImageView;

			OwningSwapChain = Other.OwningSwapChain;

			if (Other.ImageView != nullptr)
			{
				Other.OwningSwapChain->GetSwapChainImageViews().Replace(Other, *this);
			}

			Other.ImageView = VK_NULL_HANDLE;

			Other.OwningSwapChain = nullptr;
		}

		bool operator==(const UImageView& Other) const
		{
			return OwningSwapChain == Other.OwningSwapChain && ImageView == Other.ImageView;
		}

		ErrorCodes Init(VkImageViewCreateInfo& CreateInfo);
		ErrorCodes CreateImageView(VkImageViewCreateInfo& CreateInfo);

		ErrorCodes ShutDown();

		const VkImageView& GetVulkanImageView()const
		{
			return ImageView;
		}

	private:

		USwapChain* OwningSwapChain;

		VkImageView ImageView = VK_NULL_HANDLE;



	};
}
