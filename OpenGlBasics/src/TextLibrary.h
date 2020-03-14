#pragma once
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "ft2build.h"
#include FT_FREETYPE_H 

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#include <map>

#include "Shader.h"
#include "VertexArray.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "Renderer.h"

struct Character {
	GLuint     TextureID;  // ID handle of the glyph texture
	glm::ivec2 Size;       // Size of glyph
	glm::ivec2 Bearing;    // Offset from baseline to left/top of glyph
	GLuint     Advance;    // Offset to advance to next glyph
};

class TextLibrary
{
public:
	TextLibrary();
	~TextLibrary();

	void RenderText(Shader &shader, VertexArray &vao, VertexBuffer& vb, IndexBuffer &ib, std::string text, float x, float y, float scale, glm::vec3 color);
private:
	std::map<GLchar, Character> m_Characters;
};