#pragma once
#include "Tests/Test.h"

#include "ft2build.h"
#include FT_FREETYPE_H 

#include "VertexBuffer.h"
#include "VertexBufferLayout.h"
#include "Texture.h"
#include "Camera.h"

#include <memory>

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

		glm::mat4 m_Proj, m_View;
	};
}