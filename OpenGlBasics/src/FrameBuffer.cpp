#include "FrameBuffer.h"

#include <iostream>

/*
For a frame buffer to be complete, it needs:
 - At least one buffer (color, depth or stencil buffer).
 - There should be at least one color attachment.
 - All attachments should be complete as well (reserved memory).
 - Each buffer should have the same number of samples.
*/

FrameBuffer::FrameBuffer()
	: m_TextureDepthBuffer(NULL), m_RBO(NULL), m_CubeID(NULL)
{
	GLCall(glGenFramebuffers(1, &m_RendererID));
	GLCall(glBindFramebuffer(GL_FRAMEBUFFER, m_RendererID));

	GLCall(glBindFramebuffer(GL_FRAMEBUFFER, 0));
}

FrameBuffer::~FrameBuffer()
{
	GLCall(glDeleteFramebuffers(1, &m_RendererID));
}

void FrameBuffer::AddColorAttachment(unsigned int index)
{
	GLCall(glBindFramebuffer(GL_FRAMEBUFFER, m_RendererID));

	//Do Texture Loading
	GLCall(glGenTextures(1, &m_TextureColorBuffer[index]));
	GLCall(glBindTexture(GL_TEXTURE_2D, m_TextureColorBuffer[index]));

	GLCall(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 960, 540, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL));

	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));

	GLCall(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + index, GL_TEXTURE_2D, m_TextureColorBuffer[index], 0));

	GLCall(glBindFramebuffer(GL_FRAMEBUFFER, 0));
}

void FrameBuffer::AddColorFPAttachment(unsigned int index)
{
	GLCall(glBindFramebuffer(GL_FRAMEBUFFER, m_RendererID));

	//Do Texture Loading
	GLCall(glGenTextures(1, &m_TextureColorBuffer[index]));
	GLCall(glBindTexture(GL_TEXTURE_2D, m_TextureColorBuffer[index]));

	GLCall(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, 960, 540, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL));

	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));

	GLCall(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + index, GL_TEXTURE_2D, m_TextureColorBuffer[index], 0));

	GLCall(glBindFramebuffer(GL_FRAMEBUFFER, 0));
}

void FrameBuffer::AddDepthAttachment()
{
	GLCall(glBindFramebuffer(GL_FRAMEBUFFER, m_RendererID));

	//Do Texture Loading
	GLCall(glGenTextures(1, &m_TextureDepthBuffer));
	GLCall(glBindTexture(GL_TEXTURE_2D, m_TextureDepthBuffer));

	GLCall(glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, 1024, 1024, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL));

	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST));
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER));
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER));

	float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	GLCall(glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor));

	GLCall(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_TextureDepthBuffer, 0));

	GLCall(glDrawBuffer(GL_NONE));
	GLCall(glReadBuffer(GL_NONE));

	GLCall(glBindFramebuffer(GL_FRAMEBUFFER, 0));
}

void FrameBuffer::AddCubeMapAttachment(unsigned int cubeMap)
{
	GLCall(glBindFramebuffer(GL_FRAMEBUFFER, m_RendererID));

	m_CubeID = cubeMap;
	glBindTexture(GL_TEXTURE_CUBE_MAP, m_CubeID);
	GLCall(glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, m_CubeID, 0));
	GLCall(glDrawBuffer(GL_NONE));
	GLCall(glReadBuffer(GL_NONE));

	GLCall(glBindFramebuffer(GL_FRAMEBUFFER, 0));
}

void FrameBuffer::AddRenderBufferAttachment()
{
	GLCall(glBindFramebuffer(GL_FRAMEBUFFER, m_RendererID));

	//Do Renderbuffer Object Loading
	GLCall(glGenRenderbuffers(1, &m_RBO));
	GLCall(glBindRenderbuffer(GL_RENDERBUFFER, m_RBO));

	GLCall(glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, 960, 540));

	GLCall(glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_RBO));

	unsigned int attachments[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
	glDrawBuffers(2, attachments);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;

	GLCall(glBindFramebuffer(GL_FRAMEBUFFER, 0));
}

void FrameBuffer::Bind(unsigned int slot) const
{
	GLCall(glBindFramebuffer(GL_FRAMEBUFFER, m_RendererID));
}

void FrameBuffer::UnBind() const
{
	GLCall(glBindFramebuffer(GL_FRAMEBUFFER, 0));
}

void FrameBuffer::BindDepthTexture(unsigned int slot) const
{
	if (m_TextureDepthBuffer == NULL) {
		std::cout << "ERROR:: No Depth Texture Bound to FrameBuffer!" << std::endl;
	}

	GLCall(glActiveTexture(GL_TEXTURE0 + slot));
	GLCall(glBindTexture(GL_TEXTURE_2D, m_TextureDepthBuffer));
}

void FrameBuffer::UnBindDepthTexture() const
{
	GLCall(glBindTexture(GL_TEXTURE_2D, 0));
}

void FrameBuffer::BindColorTexture(unsigned int slot, unsigned int index) const
{
	if (m_TextureColorBuffer == NULL) {
		std::cout << "ERROR:: No Color Texture Bound to FrameBuffer!" << std::endl;
	}

	GLCall(glActiveTexture(GL_TEXTURE0 + slot));
	GLCall(glBindTexture(GL_TEXTURE_2D, m_TextureColorBuffer[index]));
}

void FrameBuffer::UnBindColorTexture() const
{
	GLCall(glBindTexture(GL_TEXTURE_2D, 0));
}

void FrameBuffer::BindCubeTexture(unsigned int slot) const
{
	if (m_CubeID == NULL) {
		std::cout << "ERROR:: No CubeMap Bound to FrameBuffer!" << std::endl;
	}

	GLCall(glActiveTexture(GL_TEXTURE0 + slot));
	GLCall(glBindTexture(GL_TEXTURE_CUBE_MAP, m_CubeID));
}

void FrameBuffer::UnBindCubeTexture() const
{
	GLCall(glBindTexture(GL_TEXTURE_CUBE_MAP, 0));
}

