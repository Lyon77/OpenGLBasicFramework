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
{
	GLCall(glGenFramebuffers(1, &m_RendererID));
	GLCall(glBindFramebuffer(GL_FRAMEBUFFER, m_RendererID));

	//Do Texture Loading
	GLCall(glGenTextures(1, &m_TextureColorBuffer));
	GLCall(glBindTexture(GL_TEXTURE_2D, m_TextureColorBuffer));

	GLCall(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 960, 540, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL));

	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));

	GLCall(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_TextureColorBuffer, 0));

	//Do Renderbuffer Object Loading
	GLCall(glGenRenderbuffers(1, &m_RBO));
	GLCall(glBindRenderbuffer(GL_RENDERBUFFER, m_RBO));

	GLCall(glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, 960, 540));

	GLCall(glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_RBO));


	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;

	GLCall(glBindFramebuffer(GL_FRAMEBUFFER, 0));
}

FrameBuffer::~FrameBuffer()
{
	GLCall(glDeleteFramebuffers(1, &m_RendererID));
}

void FrameBuffer::Bind(unsigned int slot) const
{
	GLCall(glBindFramebuffer(GL_FRAMEBUFFER, m_RendererID));
}

void FrameBuffer::UnBind() const
{
	GLCall(glBindFramebuffer(GL_FRAMEBUFFER, 0));
}

void FrameBuffer::BindTexture(unsigned int slot) const
{
	GLCall(glActiveTexture(GL_TEXTURE0 + slot));
	GLCall(glBindTexture(GL_TEXTURE_2D, m_TextureColorBuffer));
}

void FrameBuffer::UnBindTexture() const
{
	GLCall(glBindTexture(GL_TEXTURE_2D, 0));
}

