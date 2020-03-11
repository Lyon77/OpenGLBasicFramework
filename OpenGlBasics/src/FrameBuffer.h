#pragma once

#include "Renderer.h"

class FrameBuffer
{
private:
	unsigned int m_RendererID;
	unsigned int m_TextureColorBuffer[9] = {};
	unsigned int m_TextureDepthBuffer;
	unsigned int m_RBO;
	unsigned int m_CubeID;
public:
	FrameBuffer();
	~FrameBuffer();

	void AddColorAttachment(unsigned int index = 0);
	void AddColorFPAttachment(unsigned int index = 0);
	void AddDepthAttachment();
	void AddCubeMapAttachment(unsigned int cubeMap);
	void AddRenderBufferAttachment(unsigned int attachmentSize = 1, int width = 960, int height = 540);

	void ChangeRenderBufferStorage(int width, int height);
	void RenderToCubeMapFace(unsigned int cubeMap, unsigned int index);

	void Bind(unsigned int slot = 0) const;
	void UnBind() const;

	void BindDepthTexture(unsigned int slot = 0) const;
	void UnBindDepthTexture() const;

	void BindColorTexture(unsigned int slot = 0, unsigned int index = 0) const;
	void UnBindColorTexture() const;

	void BindCubeTexture(unsigned int slot = 0) const;
	void UnBindCubeTexture() const;
};