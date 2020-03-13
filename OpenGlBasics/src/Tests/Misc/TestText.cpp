#include "TestText.h"

#include <iostream>

#include "Renderer.h"
#include "Window.h"
#include "imgui/imgui.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

namespace test {

	TestText::TestText()
		: m_Pos(glm::vec2(25.0f)), m_Scale(2.0f)
	{
		
		m_Proj = glm::ortho(0.0f, (float)Window::GetInstance().GetWidth(), 0.0f, (float)Window::GetInstance().GetHeight());

		//Depth Testing
		GLCall(glEnable(GL_DEPTH_TEST));

		//Blending for transparent bits in the texture
		GLCall(glEnable(GL_BLEND));
		GLCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));

		//create vertex array
		m_VAO = std::make_unique<VertexArray>();

		//create vertex buffer
		m_VertexBuffer = std::make_unique<VertexBuffer>(nullptr, 4 * 4 * sizeof(float)); //3 values, 4, points

		//set vertex buffer to array
		VertexBufferLayout layout;
		layout.Push<float>(2);
		layout.Push<float>(2);
		m_VAO->AddBuffer(*m_VertexBuffer, layout);

		unsigned int indices[] = {
			0, 1, 2,
			2, 3, 0
		};

		m_IndexBuffer = std::make_unique<IndexBuffer>(indices, 6);

		//create Vertex and Fragment source
		m_Shader = std::make_unique<Shader>("res/shaders/Text.shader");
		m_Shader->Bind();
		m_Shader->SetUniform1i("u_Text", 0);
		m_Shader->SetUniformMat4f("u_Projection", m_Proj);
		m_Shader->UnBind();

		// Load Library and Faces. The functions will return non zero if failed
		FT_Library ft;
		if (FT_Init_FreeType(&ft))
			std::cout << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;

		FT_Face face;
		if (FT_New_Face(ft, "../../../Windows/Fonts/arial.ttf", 0, &face))
			std::cout << "ERROR::FREETYPE: Failed to load font" << std::endl;
			std::cout << "May Have to Change Font File Location" << std::endl;

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

	TestText::~TestText()
	{
		glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
	}

	void TestText::OnUpdate(float deltaTime)
	{
	}

	void TestText::OnRender()
	{
		GLCall(glClearColor(0.1f, 0.1f, 0.1f, 1.0f));
		GLCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT));

		Renderer renderer;

		// Load the box
		{
			m_Shader->Bind();
			m_Shader->SetUniform3f("u_TextColor", m_Color[0], m_Color[1], m_Color[2]);
			
			std::string s = "Test Output";
			float x = m_Pos.x;
			float y = m_Pos.y;
			float scale = m_Scale;

			// Iterate through all characters
			std::string::const_iterator c;
			for (c = s.begin(); c != s.end(); c++)
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
				m_VertexBuffer->UpdateSubData(position, 4 * 4 * sizeof(float));
				// Render quad
				renderer.Draw(*m_VAO, *m_IndexBuffer, *m_Shader);
				// Now advance cursors for next glyph (note that advance is number of 1/64 pixels)
				x += (ch.Advance >> 6) * scale; // Bitshift by 6 to get value in pixels (2^6 = 64)
			}
		}

	}

	void TestText::OnImGuiRender()
	{
		ImGui::TextWrapped("Welcome to the Text Test Enviroment. There are more setting options below.");

		ImGui::SliderFloat("Translate X", &m_Pos.x, 0.0f, 960.0f);
		ImGui::SliderFloat("Translate Y", &m_Pos.y, 0.0f, 540.0f);
		ImGui::SliderFloat("Scale", &m_Scale, 0.2f, 4.0f);
		ImGui::ColorEdit4("Color", m_Color);
		
		//displays framerate
		ImGui::TextWrapped("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
	}
}