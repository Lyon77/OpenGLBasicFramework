#include "TestTexture2D.h"

#include "Renderer.h"
#include "imgui/imgui.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

namespace test {
	TestTexture2D::TestTexture2D()
		: m_Proj(glm::ortho(0.0f, 960.0f, 0.0f, 540.0f, -1.0f, 500.0f)), m_View(glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, 0))), //glm::ortho(0.0f, 960.0f, 0.0f, 540.0f, -1.0f, 1.0f)
		m_TranslationA(200, 200, 0), m_TranslationB(400, 200, 0)
	{

		//Define Triangle
		float positions[] = {
			 -50.0f, -50.0f, 0.0f, 0.0f,
			  50.0f, -50.0f, 2.0f, 0.0f,
			  50.0f,  50.0f, 2.0f, 2.0f,
			 -50.0f,  50.0f, 0.0f, 2.0f
		};

		unsigned int indicies[] = {
			0, 1, 2,
			2, 3, 0
		};

		//Blending for transparent bits in the texture
		GLCall(glEnable(GL_BLEND));
		GLCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));

		//create vertex array
		m_VAO = std::make_unique<VertexArray>();

		//create vertex buffer
		m_VertexBuffer = std::make_unique<VertexBuffer>(positions, 4 * 4 * sizeof(float));

		//set vertex buffer to array
		VertexBufferLayout layout;
		layout.Push<float>(2);
		layout.Push<float>(2);
		m_VAO->AddBuffer(*m_VertexBuffer, layout);

		//create index buffer
		m_IndexBuffer = std::make_unique<IndexBuffer>(indicies, 6);

		////create projection matrix
		//m_Proj = glm::ortho(0.0f, 960.0f, 0.0f, 540.0f, -1.0f, 1.0f);
		////create view matrix
		//m_View = glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, 0));

		//create Vertex and Fragment source
		m_Shader = std::make_unique<Shader>("res/shaders/Basic.shader");
		m_Shader->Bind();

		//SetTexture
		m_Texture = std::make_unique<Texture>("res/textures/Room.png");
		m_Shader->SetUniform1i("u_Texture", 0);

		////UnBind just to check functionality
		//m_VAO->UnBind();
		//m_Shader->UnBind();
		//vb.UnBind();
		//m_IndexBuffer->UnBind();
	}
	TestTexture2D::~TestTexture2D()
	{
	}
	void TestTexture2D::OnUpdate(float deltaTime)
	{
	}
	void TestTexture2D::OnRender()
	{
		GLCall(glClearColor(0.0f, 0.0f, 0.0f, 1.0f));
		GLCall(glClear(GL_COLOR_BUFFER_BIT));

		Renderer renderer;

		m_Texture->Bind();

		{
			m_Shader->Bind();
			//create model matrix
			glm::mat4 model = glm::translate(glm::mat4(1.0f), m_TranslationA);
			//construt model view projection
			glm::mat4 mvp = m_Proj * m_View * model;

			m_Shader->SetUniformMat4f("u_MVP", mvp);

			//draw first texture
			renderer.Draw(*m_VAO, *m_IndexBuffer, *m_Shader);
		}

		{
			m_Shader->Bind();
			//create model matrix
			glm::mat4 model = glm::translate(glm::mat4(1.0f), m_TranslationB);
			//construt model view projection
			glm::mat4 mvp = m_Proj * m_View * model;

			m_Shader->SetUniformMat4f("u_MVP", mvp);

			//draw second texture
			renderer.Draw(*m_VAO, *m_IndexBuffer, *m_Shader);
		}
	}
	void TestTexture2D::OnImGuiRender()
	{
		//translates imageA
		ImGui::SliderFloat3("Translation A", &m_TranslationA.x, 0.0f, 960.0f);
		//translates imageB
		ImGui::SliderFloat3("Translation B", &m_TranslationB.x, 0.0f, 960.0f);
		//displays framerate
		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
	}
}