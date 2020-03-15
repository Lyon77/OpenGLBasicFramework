#include "TextLibrary.h"

#include <iostream>

TextLibrary::TextLibrary(std::string path)
{
	// Load Library and Faces. The functions will return non zero if failed
	FT_Library ft;
	if (FT_Init_FreeType(&ft))
		std::cout << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;

	FT_Face face;
	if (FT_New_Face(ft, path.c_str(), 0, &face))
	{
		std::cout << "ERROR::FREETYPE: Failed to load font" << std::endl;
		std::cout << "May Have to Change Font File Location" << std::endl;
	}

	FT_Set_Pixel_Sizes(face, 0, 48); // Dynamically set width based on height

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1); // Disable byte-alignment restriction

	for (GLubyte c = 0; c < 128; c++)
	{
		// Load character glyph 
		if (FT_Load_Char(face, c, FT_LOAD_RENDER))
		{
			std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
			continue;
		}
		// Generate texture
		GLuint texture;
		glGenTextures(1, &texture);
		glBindTexture(GL_TEXTURE_2D, texture);
		glTexImage2D(
			GL_TEXTURE_2D,
			0,
			GL_RED,
			face->glyph->bitmap.width,
			face->glyph->bitmap.rows,
			0,
			GL_RED,
			GL_UNSIGNED_BYTE,
			face->glyph->bitmap.buffer
		);
		// Set texture options
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		// Now store character for later use
		Character character = {
			texture,
			glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
			glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
			face->glyph->advance.x
		};
		m_Characters.insert(std::pair<GLchar, Character>(c, character));
	}

	// Clear FreeType Resources
	FT_Done_Face(face);
	FT_Done_FreeType(ft);
}

TextLibrary::~TextLibrary()
{
}

void TextLibrary::RenderText(Shader& shader, VertexArray& vao, VertexBuffer& vb, IndexBuffer& ib, std::string text, float x, float y, float scale, glm::vec3 color)
{
	Renderer renderer;

	// Iterate through all characters
	std::string::const_iterator c;
	for (c = text.begin(); c != text.end(); c++)
	{
		Character ch = m_Characters[*c];

		float xPos = x + ch.Bearing.x * scale;
		float yPos = y - (ch.Size.y - ch.Bearing.y) * scale;

		float w = ch.Size.x * scale;
		float h = ch.Size.y * scale;

		float position[] = {
			xPos    , yPos + h, 0.0, 0.0,
			xPos    , yPos    , 0.0, 1.0,
			xPos + w, yPos    , 1.0, 1.0,
			xPos + w, yPos + h, 1.0, 0.0
		};

		// Render glyph texture over quad
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, ch.TextureID);
		// Update content of VBO memory
		vb.UpdateSubData(position, 4 * 4 * sizeof(float));
		// Render quad
		renderer.Draw(vao, ib, shader);
		// Now advance cursors for next glyph (note that advance is number of 1/64 pixels)
		x += (ch.Advance >> 6)* scale; // Bitshift by 6 to get value in pixels (2^6 = 64)
	}
}
