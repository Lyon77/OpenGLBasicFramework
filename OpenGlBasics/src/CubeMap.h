#pragma once

#include <vector>

#include "Renderer.h"

class CubeMap {
private:
	unsigned int m_RendererID;
	std::vector<std::string> m_FilePaths;
	unsigned char* m_LocalBuffer;
	int m_Width, m_Height, m_BPP;
public:
	CubeMap(const std::vector<std::string> textures_faces);
	~CubeMap();

	void Bind(unsigned int slot = 0) const;
	void UnBind() const;

	inline int GetWidth() const { return m_Width; }
	inline int GetHeight() const { return m_Height; }
};