


#pragma once
#include "glad/glad.h"
#include "Vector2D.h"

struct TextureBuffer;
struct RenderBuffer;

//Function templates for generation, allows you to pass in a function as long as it follows this format
using TextureGenerator = unsigned int(*)(const TextureBuffer* TexData, const Vector2D& ScreenParams);
using RenderGenerator = unsigned int(*)(const RenderBuffer* RenderData, const Vector2D& ScreenParams);

struct TextureBuffer
{
public:

	/**
	 * The location it should target, i.e. GL_TEXTURE_2D
	 */
	GLenum Target;

	/**
	 * The function it should call to generate the texture, could be a lambda or one of the preset
	 */
	TextureGenerator GenTexture;

	/**
	 * Something like RGB
	 */
	GLenum Format;

	TextureBuffer(const GLenum InTarget, const GLenum InFormat, const TextureGenerator InGenTexture) :
	Target(InTarget), GenTexture(InGenTexture), Format(InFormat) {};
};

struct RenderBuffer
{
public:

	/**
	 * The location it should target, i.e. GL_RENDERBUFFER
	 */
	GLenum Target;

	/**
	 * 	The function it should call to generate the renderbuffer, could be a lambda or one of the preset
	 */
	RenderGenerator GenRenderer;

	/**
	 * Format to load, i.e. GL_DEPTH28_Stencil_8
	 */
	GLenum Format;


	/**
	 * What should be attached to the renderbuffer, should match format, i.e. GL_DEPTH_STENCIL_ATTACHMENT
	 */
	GLenum Attachment;

	RenderBuffer(const GLenum InTarget, const GLenum InFormat, const GLenum InAttachment, const RenderGenerator InGenRenderer) :
		Target(InTarget), GenRenderer(InGenRenderer), Format(InFormat), Attachment(InAttachment) {
	};
};

struct Buffers
{
public:
	unsigned int FBO;
	unsigned int Texture;
	unsigned int RBO;

	Buffers(const Buffers& Other) = default;

	explicit Buffers(const unsigned int InFBO = -1, const unsigned int InTexture = -1, const unsigned int InRBO = -1)
	: FBO(InFBO), Texture(InTexture), RBO(InRBO) {};
};

class FrameBufferFactory
{
public:


	/**
	 * @return A frame buffer with no attachments.
	 */
	static Buffers GenerateFrameBuffer();

	/**
	 * @param TexData How should it generate the texture
	 * @param ScreenParams Screen size
	 * @return A frame buffer with a color buffer
	 */
	static Buffers GenerateFrameBuffer(TextureBuffer* TexData, Vector2D ScreenParams = Vector2D(800, 600));

	/**
	 *
	 * @param TexData How should it generate the texture
	 * @param RenderData how it should generate the render buffer
	 * @param ScreenParams Screen size
	 * @return A frame buffer with a color buffer and a Depth and/or stencil buffer
	 */
	static Buffers GenerateFrameBuffer(TextureBuffer* TexData, RenderBuffer* RenderData,
	                                   Vector2D ScreenParams = Vector2D(800, 600));

	/**
	 * @return a color buffer for a texture 2D
	 */
	static unsigned int GenerateTex2D(const TextureBuffer* TexData, const Vector2D& ScreenParams = Vector2D(800, 600));

	/**
	 * @return a color buffer for a multisampled texture 2D
	 */
	static unsigned int GenerateTex2DMultiSampled(const TextureBuffer* TexData, const Vector2D& ScreenParams = Vector2D(800, 600));

	/**
	 * @return a render buffer
	 */
	static unsigned int GenerateRenderer(const RenderBuffer* RenderData, const Vector2D& ScreenParams = Vector2D(800, 600));

	/**
	 * @return a multisampled render buffer.
	 */
	static unsigned int GenerateRendererMultiSampled(const RenderBuffer* RenderData, const Vector2D& ScreenParams = Vector2D(800, 600));

	private:
	static void IsBufferComplete();
};
