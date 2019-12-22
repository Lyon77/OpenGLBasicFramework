#include "TestAdvancedTextures.h"

#include <iostream>

#include "Renderer.h"
#include "imgui/imgui.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

namespace test {
	float i = 0.0f;

	TestAdvancedTextures::TestAdvancedTextures()
		: m_Proj(glm::ortho(0.0f, 960.0f, 0.0f, 540.0f, -1.0f, 1.0f)), m_View(glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, 0))),
		m_TranslationA(200, 200, 0)
	{

		//Define Triangle
		float positions[] = {
			//position           //color             //texture
			 -100.0f, -100.0f,   1.0f, 0.0f, 0.0f,   0.0f, 0.0f,
			  100.0f, -100.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f,
			  100.0f,  100.0f,   0.0f, 0.0f, 1.0f,   1.0f, 1.0f,
			 -100.0f,  100.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f
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
		m_VertexBuffer = std::make_unique<VertexBuffer>(positions, 7 * 4 * sizeof(float));

		//set vertex buffer to array
		VertexBufferLayout layout;
		layout.Push<float>(2);
		layout.Push<float>(3);
		layout.Push<float>(2);
		m_VAO->AddBuffer(*m_VertexBuffer, layout);

		//create index buffer
		m_IndexBuffer = std::make_unique<IndexBuffer>(indicies, 6);

		//create Vertex and Fragment source
		m_Shader = std::make_unique<Shader>("res/shaders/Advanced.shader");
		m_Shader->Bind();

		//SetTexture
		m_Texture = std::make_unique<Texture>("res/textures/Room.png");
		m_Shader->SetUniform1i("u_Texture", 0);
	}
	TestAdvancedTextures::~TestAdvancedTextures()
	{
	}
	void TestAdvancedTextures::OnUpdate(float deltaTime)
	{
	}
	void TestAdvancedTextures::OnRender()
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
	}
	void TestAdvancedTextures::OnImGuiRender()
	{
		//translates imageA
		ImGui::SliderFloat3("Translation A", &m_TranslationA.x, 0.0f, 960.0f);
		//displays framerate
		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
	}
}