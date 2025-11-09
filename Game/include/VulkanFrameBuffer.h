
#pragma once

#include "ErrorCodes.h"
#include "GraphicsAPIConstructor.h"


namespace Vulkan
{
	class ULogicalDevice;
	class UImageView;
}

namespace Vulkan
{

	class UFrameBuffer
	{
	public:

		UFrameBuffer() = default;

		UFrameBuffer(VkFramebufferCreateInfo CreateInfo);
		~UFrameBuffer();

		UFrameBuffer(const UFrameBuffer& InCopy)
		{
			Copy(InCopy);
		}

		UFrameBuffer& operator=(const UFrameBuffer& Other)
		{
			if (this != &Other)
			{
				Copy(Other);
			}
			return *this;
		}

		void Move(UFrameBuffer&& Other)
		{
			if (FrameBuffer != VK_NULL_HANDLE) ShutDown();

			FrameBuffer = Other.FrameBuffer;
			Other.FrameBuffer = VK_NULL_HANDLE;
		}

		UFrameBuffer(UFrameBuffer&& Other) noexcept
		{
			Move(std::move(Other));
		}

		UFrameBuffer& operator=(UFrameBuffer&& Other) noexcept
		{
			if (this != &Other)
			{
				Move(std::move(Other));
			}
			return *this;
		}

		void Copy(const UFrameBuffer& Copy);


		ErrorCodes Init(VkFramebufferCreateInfo CreateInfo);

		ErrorCodes ShutDown();

		const VkFramebuffer GetFrameBuffer()const
		{
			return FrameBuffer;
		}


	private:

		VkFramebuffer FrameBuffer = VK_NULL_HANDLE;

	};
}
