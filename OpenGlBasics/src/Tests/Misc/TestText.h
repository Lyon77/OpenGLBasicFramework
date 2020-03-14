#pragma once
#include "Tests/Test.h"

#include "ft2build.h"
#include FT_FREETYPE_H 

#include "VertexBuffer.h"
#include "VertexBufferLayout.h"
#include "Texture.h"
#include "TextLibrary.h"
#include "Camera.h"

#include <memory>
#include <map>

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

		std::unique_ptr <TextLibrary> m_TextLibrary;

		glm::mat4 m_Proj;

		glm::vec2 m_Pos;
		float m_Scale;
		float m_Color[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
	};
}