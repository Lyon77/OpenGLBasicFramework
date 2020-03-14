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

		m_TextLibrary = std::make_unique<TextLibrary>();
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
		
			glm::vec3 color = glm::vec3(m_Color[0], m_Color[1], m_Color[2]);
			m_TextLibrary->RenderText(*m_Shader, *m_VAO, *m_VertexBuffer, * m_IndexBuffer, "Display Text", m_Pos.x, m_Pos.y, m_Scale, color);
		}

	}

	void TestText::OnImGuiRender()
	{
		ImGui::TextWrapped("Welcome to the Text Test Enviroment. The Text will only work for English character and punctuation.");

		ImGui::SliderFloat("Translate X", &m_Pos.x, 0.0f, 960.0f);
		ImGui::SliderFloat("Translate Y", &m_Pos.y, 0.0f, 540.0f);
		ImGui::SliderFloat("Scale", &m_Scale, 0.2f, 4.0f);
		ImGui::ColorEdit4("Color", m_Color);
		
		//displays framerate
		ImGui::TextWrapped("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
	}
}