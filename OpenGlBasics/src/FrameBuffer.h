#pragma once

#include "Renderer.h"

class FrameBuffer
{
private:
	unsigned int m_RendererID;
	unsigned int m_TextureColorBuffer;
	unsigned int m_RBO;
public:
	FrameBuffer();
	~FrameBuffer();

	void Bind(unsigned int slot = 0) const;
	void UnBind() const;

	void BindTexture(unsigned int slot = 0) const;
	void UnBindTexture() const;
};