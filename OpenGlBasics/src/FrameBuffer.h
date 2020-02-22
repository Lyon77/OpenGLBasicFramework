#pragma once

#include "Renderer.h"

class FrameBuffer
{
private:
	unsigned int m_RendererID;
	unsigned int m_TextureColorBuffer;
	unsigned int m_TextureDepthBuffer;
	unsigned int m_RBO;
	unsigned int m_CubeID;
public:
	FrameBuffer();
	~FrameBuffer();

	void AddColorAttachment();
	void AddColorFPAttachment();
	void AddDepthAttachment();
	void AddCubeMapAttachment(unsigned int cubeMap);

	void Bind(unsigned int slot = 0) const;
	void UnBind() const;

	void BindDepthTexture(unsigned int slot = 0) const;
	void UnBindDepthTexture() const;

	void BindColorTexture(unsigned int slot = 0) const;
	void UnBindColorTexture() const;

	void BindCubeTexture(unsigned int slot = 0) const;
	void UnBindCubeTexture() const;
};