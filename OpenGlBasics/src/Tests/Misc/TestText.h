#pragma once
#include "Tests/Test.h"

#include "ft2build.h"
#include FT_FREETYPE_H 

#include "VertexBuffer.h"
#include "VertexBufferLayout.h"
#include "Texture.h"
#include "Camera.h"

#include <memory>
#include <map>

struct Character {
	GLuint     TextureID;  // ID handle of the glyph texture
	glm::ivec2 Size;       // Size of glyph
	glm::ivec2 Bearing;    // Offset from baseline to left/top of glyph
	GLuint     Advance;    // Offset to advance to next glyph
};

namespace test {

	class TestText : public Test {
	public:
		TestText();
		~TestText();

		void OnUpdate(float deltaTime) override;
		void OnRender() override;
		void OnImGuiRender() override;
	private:
		std::unique_ptr <VertexArray> m_VAO;
		std::unique_ptr <VertexBuffer> m_VertexBuffer;
		std::unique_ptr <IndexBuffer> m_IndexBuffer;
		std::unique_ptr <Shader> m_Shader;

		glm::mat4 m_Proj;

		std::map<GLchar, Character> m_Characters;
		glm::vec2 m_Pos;
		float m_Scale;
		float m_Color[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
	};
}