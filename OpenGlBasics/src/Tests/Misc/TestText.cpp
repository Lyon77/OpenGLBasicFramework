#include "TestText.h"

#include <iostream>

#include "Renderer.h"
#include "imgui/imgui.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

namespace test {

	TestText::TestText()
		: m_Proj(glm::perspective(glm::radians(45.0f), 960.0f / 540.0f, 0.1f, 100.0f)), m_View(glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, -3.0))) //glm::perspective(glm::radians(45.0f), 960.0f / 540.0f, 0.1f, 500.0f)
	{

		//Define Triangle
		// FIX ME: try to get the positions to be from 960 by 540
		float positions[] = {
			//position             //normal          	  //texture

			//Back
			-0.5f, -0.5f, -0.5f,   0.0f,  0.0f, -1.0f,	  0.0f, 0.0f,
			 0.5f, -0.5f, -0.5f,   0.0f,  0.0f, -1.0f,	  1.0f, 0.0f,
			 0.5f,  0.5f, -0.5f,   0.0f,  0.0f, -1.0f,	  1.0f, 1.0f,
			-0.5f,  0.5f, -0.5f,   0.0f,  0.0f, -1.0f,	  0.0f, 1.0f,

			//Front
			-0.5f, -0.5f,  0.5f,   0.0f,  0.0f,  1.0f,	  0.0f, 0.0f,
			 0.5f, -0.5f,  0.5f,   0.0f,  0.0f,  1.0f,	  1.0f, 0.0f,
			 0.5f,  0.5f,  0.5f,   0.0f,  0.0f,  1.0f,	  1.0f, 1.0f,
			-0.5f,  0.5f,  0.5f,   0.0f,  0.0f,  1.0f,	  0.0f, 1.0f,

			//Left
			-0.5f, -0.5f,  0.5f,  -1.0f,  0.0f,  0.0f,	  0.0f, 0.0f,
			-0.5f, -0.5f, -0.5f,  -1.0f,  0.0f,  0.0f,	  1.0f, 0.0f,
			-0.5f,  0.5f, -0.5f,  -1.0f,  0.0f,  0.0f,	  1.0f, 1.0f,
			-0.5f,  0.5f,  0.5f,  -1.0f,  0.0f,  0.0f,	  0.0f, 1.0f,

			//Right
			 0.5f, -0.5f,  0.5f,   1.0f,  0.0f,  0.0f,	  0.0f, 0.0f,
			 0.5f, -0.5f, -0.5f,   1.0f,  0.0f,  0.0f,	  1.0f, 0.0f,
			 0.5f,  0.5f, -0.5f,   1.0f,  0.0f,  0.0f,	  1.0f, 1.0f,
			 0.5f,  0.5f,  0.5f,   1.0f,  0.0f,  0.0f,	  0.0f, 1.0f,

			 //Top
			  0.5f,  0.5f,  0.5f,   0.0f,  1.0f,  0.0f,	  0.0f, 0.0f,
			  0.5f,  0.5f, -0.5f,   0.0f,  1.0f,  0.0f,	  1.0f, 0.0f,
			 -0.5f,  0.5f, -0.5f,   0.0f,  1.0f,  0.0f,	  1.0f, 1.0f,
			 -0.5f,  0.5f,  0.5f,   0.0f,  1.0f,  0.0f,	  0.0f, 1.0f,

			 //Bottom			
			  0.5f, -0.5f,  0.5f,   0.0f, -1.0f,  0.0f,	  0.0f, 0.0f,
			  0.5f, -0.5f, -0.5f,   0.0f, -1.0f,  0.0f,	  1.0f, 0.0f,
			 -0.5f, -0.5f, -0.5f,   0.0f, -1.0f,  0.0f,	  1.0f, 1.0f,
			 -0.5f, -0.5f,  0.5f,   0.0f, -1.0f,  0.0f,	  0.0f, 1.0f,
		};

		unsigned int indicies[] = {
			0, 2, 1,
			2, 0, 3,

			4, 5, 6,
			6, 7, 4,

			8, 10, 9,
			10, 8, 11,

			12, 13, 14,
			14, 15, 12,

			16, 17, 18,
			18, 19, 16,

			20, 22, 21,
			22, 20, 23
		};

		//Depth Testing
		GLCall(glEnable(GL_DEPTH_TEST));

		//Blending for transparent bits in the texture
		GLCall(glEnable(GL_BLEND));
		GLCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));

		//create vertex array
		m_VAO = std::make_unique<VertexArray>();

		//create vertex buffer
		m_VertexBuffer = std::make_unique<VertexBuffer>(positions, 8 * 4 * 6 * sizeof(float)); //3 values, 4, points, 6 faces

		//set vertex buffer to array
		VertexBufferLayout layout;
		layout.Push<float>(3);
		layout.Push<float>(3);
		layout.Push<float>(2);
		m_VAO->AddBuffer(*m_VertexBuffer, layout);

		//create index buffer
		m_IndexBuffer = std::make_unique<IndexBuffer>(indicies, 6 * 6);

		//create Vertex and Fragment source
		m_Shader = std::make_unique<Shader>("res/shaders/Text.shader");
	}

	TestText::~TestText()
	{
	}

	void TestText::OnUpdate(float deltaTime)
	{
	}

	void TestText::OnRender()
	{
		GLCall(glClearColor(0.0f, 0.0f, 0.0f, 1.0f));
		GLCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT));

		Renderer renderer;

		// Load the box
		{
		}

	}

	void TestText::OnImGuiRender()
	{
		ImGui::TextWrapped("Welcome to the Text Test Enviroment. There are more setting options below.");


		//displays framerate
		ImGui::TextWrapped("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
	}
}