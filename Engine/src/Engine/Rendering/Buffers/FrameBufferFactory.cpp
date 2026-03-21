


#include "FrameBufferFactory.h"
#include <iostream>

Buffers FrameBufferFactory::GenerateFrameBuffer()
{
	unsigned int fbo;

	glGenFramebuffers(1, &fbo);

	glBindFramebuffer(GL_FRAMEBUFFER, fbo);

	return Buffers(fbo);
}

Buffers FrameBufferFactory::GenerateFrameBuffer(TextureBuffer* TexData, const Vector2D ScreenParams)
{
	const Buffers fbo = GenerateFrameBuffer();

	//Call the input function to generate a texture
	const unsigned int texture = TexData->GenTexture(TexData, ScreenParams);

	IsBufferComplete();

	return Buffers(fbo.FBO, texture);
}

Buffers FrameBufferFactory::GenerateFrameBuffer(TextureBuffer* TexData, RenderBuffer* RenderData, const Vector2D ScreenParams)
{
	const Buffers fbo = GenerateFrameBuffer();

	const unsigned int texture = TexData->GenTexture(TexData, ScreenParams);

	const unsigned int rbo = RenderData->GenRenderer(RenderData, ScreenParams);

	IsBufferComplete();

	return Buffers(fbo.FBO, texture, rbo);
}

unsigned int FrameBufferFactory::GenerateTex2D(const TextureBuffer* TexData, const Vector2D& ScreenParams)
{
	unsigned int texture;
	glGenTextures(1, &texture);
	glBindTexture(TexData->Target, texture);

	glTexImage2D(TexData->Target, 0, TexData->Format, static_cast<GLsizei>(ScreenParams.X), static_cast<GLsizei>(ScreenParams.Y),
		0, TexData->Format, GL_UNSIGNED_BYTE, nullptr);

	//TODO make a way for this to be passed in, so you have more control
	glTexParameteri(TexData->Target, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(TexData->Target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(TexData->Target, 0);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, TexData->Target, texture, 0);

	return texture;
}

unsigned int FrameBufferFactory::GenerateTex2DMultiSampled(const TextureBuffer* TexData, const Vector2D& ScreenParams)
{
	unsigned int texture;
	glGenTextures(1, &texture);
	glBindTexture(TexData->Target, texture);

	glTexImage2DMultisample(TexData->Target, 4, TexData->Format, static_cast<GLsizei>(ScreenParams.X), static_cast<GLsizei>(ScreenParams.Y),
		GL_TRUE);
	glBindTexture(TexData->Target, 0);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, TexData->Target, texture, 0);

	return texture;
}

unsigned int FrameBufferFactory::GenerateRenderer(const RenderBuffer* RenderData, const Vector2D& ScreenParams)
{
	unsigned int rbo;
	glGenRenderbuffers(1, &rbo);
	glBindRenderbuffer(RenderData->Target, rbo);

	glRenderbufferStorage(RenderData->Target, RenderData->Format, static_cast<GLsizei>(ScreenParams.X), static_cast<GLsizei>(ScreenParams.Y));

	glFramebufferRenderbuffer(GL_FRAMEBUFFER, RenderData->Attachment, RenderData->Target, rbo);

	return rbo;
}

unsigned int FrameBufferFactory::GenerateRendererMultiSampled(const RenderBuffer* RenderData, const Vector2D& ScreenParams)
{
	unsigned int rbo;
	glGenRenderbuffers(1, &rbo);
	glBindRenderbuffer(RenderData->Target, rbo);

	glRenderbufferStorageMultisample(RenderData->Target, 4, RenderData->Format, static_cast<GLsizei>(ScreenParams.X), static_cast<GLsizei>(ScreenParams.Y));

	glFramebufferRenderbuffer(GL_FRAMEBUFFER, RenderData->Attachment, RenderData->Target, rbo);

	return rbo;
}

void FrameBufferFactory::IsBufferComplete()
{
#if DEBUG
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
	}
#endif
}
