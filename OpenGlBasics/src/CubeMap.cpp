#include "CubeMap.h"

#include <iostream>

#include "stb_image/stb_header.h"

CubeMap::CubeMap(const std::vector<std::string> textures_faces)
	: m_RendererID(0), m_FilePaths(textures_faces), m_LocalBuffer(nullptr), m_Width(0), m_Height(0), m_BPP(0)
{
	//Seems like you need to flip when using png and not jpg
	stbi_set_flip_vertically_on_load(false);

	//load texture
	GLCall(glGenTextures(1, &m_RendererID));
	GLCall(glBindTexture(GL_TEXTURE_CUBE_MAP, m_RendererID));

	//texture parameters
	GLCall(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
	GLCall(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
	GLCall(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
	GLCall(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
	GLCall(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE));

	//send to opengl
	for (GLuint i = 0; i < textures_faces.size(); i++)
	{
		m_LocalBuffer = stbi_load(textures_faces[i].c_str(), &m_Width, &m_Height, &m_BPP, 4);

		if (m_LocalBuffer) {
			GLCall(glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGBA8, m_Width, m_Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, m_LocalBuffer));
			stbi_image_free(m_LocalBuffer);
		}
		else
		{
			std::cout << "Cubemap texture failed to load at path: " << textures_faces[i] << std::endl;
			stbi_image_free(m_LocalBuffer);
		}
	}

	GLCall(glBindTexture(GL_TEXTURE_CUBE_MAP, 0));

}

CubeMap::~CubeMap()
{
	GLCall(glDeleteTextures(1, &m_RendererID));
}

void CubeMap::Bind(unsigned int slot) const
{
	GLCall(glActiveTexture(GL_TEXTURE0 + slot));
	GLCall(glBindTexture(GL_TEXTURE_CUBE_MAP, m_RendererID));
}

void CubeMap::UnBind() const
{
	GLCall(glBindTexture(GL_TEXTURE_CUBE_MAP, 0));
}
