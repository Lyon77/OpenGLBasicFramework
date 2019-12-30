#include "TestAdvancedTextures.h"

#include <iostream>

#include "Renderer.h"
#include "imgui/imgui.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

namespace test {
	TestAdvancedTextures::TestAdvancedTextures()
		: m_Proj(glm::perspective(glm::radians(45.0f), 960.0f / 540.0f, 0.1f, 100.0f)), m_View(glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, -3.0))), //glm::perspective(glm::radians(45.0f), 960.0f / 540.0f, 0.1f, 500.0f)
		m_TranslationA(0, 0, 0), m_FOV(45.0f), m_YawPitch(glm::vec2(0.0f, 0.0f)), m_Speed(2.5f)
	{
		//Define Triangle
		// FIX ME: try to get the positions to be from 960 by 540
		float positions[] = {
			//position             //color             //texture

			//Back
			-0.5f, -0.5f, -0.5f,   1.0f, 0.5f, 0.5f,   0.0f, 0.0f,
			 0.5f, -0.5f, -0.5f,   0.5f, 1.0f, 0.5f,   1.0f, 0.0f,
			 0.5f,  0.5f, -0.5f,   0.5f, 0.0f, 1.0f,   1.0f, 1.0f,
			-0.5f,  0.5f, -0.5f,   1.0f, 0.5f, 0.5f,   0.0f, 1.0f,

			//Front
			-0.5f, -0.5f,  0.5f,   1.0f, 0.5f, 0.5f,   0.0f, 0.0f,
			 0.5f, -0.5f,  0.5f,   0.5f, 1.0f, 0.5f,   1.0f, 0.0f,
			 0.5f,  0.5f,  0.5f,   0.5f, 0.0f, 1.0f,   1.0f, 1.0f,
			-0.5f,  0.5f,  0.5f,   1.0f, 0.5f, 0.5f,   0.0f, 1.0f,

			//Left
			-0.5f, -0.5f,  0.5f,   1.0f, 0.5f, 0.5f,   0.0f, 0.0f,
			-0.5f, -0.5f, -0.5f,   0.5f, 1.0f, 0.5f,   1.0f, 0.0f,
			-0.5f,  0.5f, -0.5f,   0.5f, 0.0f, 1.0f,   1.0f, 1.0f,
			-0.5f,  0.5f,  0.5f,   1.0f, 0.5f, 0.5f,   0.0f, 1.0f,

			//Right
			 0.5f, -0.5f,  0.5f,   1.0f, 0.5f, 0.5f,   0.0f, 0.0f,
			 0.5f, -0.5f, -0.5f,   0.5f, 1.0f, 0.5f,   1.0f, 0.0f,
			 0.5f,  0.5f, -0.5f,   0.5f, 0.0f, 1.0f,   1.0f, 1.0f,
			 0.5f,  0.5f,  0.5f,   1.0f, 0.5f, 0.5f,   0.0f, 1.0f,

			//Top
			 0.5f,  0.5f,  0.5f,   1.0f, 0.5f, 0.5f,   0.0f, 0.0f,
			 0.5f,  0.5f, -0.5f,   0.5f, 1.0f, 0.5f,   1.0f, 0.0f,
			-0.5f,  0.5f, -0.5f,   0.5f, 0.0f, 1.0f,   1.0f, 1.0f,
			-0.5f,  0.5f,  0.5f,   1.0f, 0.5f, 0.5f,   0.0f, 1.0f,

			//Bottom
			 0.5f, -0.5f,  0.5f,   1.0f, 0.5f, 0.5f,   0.0f, 0.0f,
			 0.5f, -0.5f, -0.5f,   0.5f, 1.0f, 0.5f,   1.0f, 0.0f,
			-0.5f, -0.5f, -0.5f,   0.5f, 0.0f, 1.0f,   1.0f, 1.0f,
			-0.5f, -0.5f,  0.5f,   1.0f, 0.5f, 0.5f,   0.0f, 1.0f
		};

		unsigned int indicies[] = {
			0, 1, 2,
			2, 3, 0,

			4, 5, 6,
			6, 7, 4,

			8, 9, 10,
			10, 11, 8,

			12, 13, 14,
			14, 15, 12,

			16, 17, 18,
			18, 19, 16,

			20, 21, 22,
			22, 23, 20
		};

		

		//Depth Testing
		GLCall(glEnable(GL_DEPTH_TEST));

		//Blending for transparent bits in the texture
		GLCall(glEnable(GL_BLEND));
		GLCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));

		//create vertex array
		m_VAO = std::make_unique<VertexArray>();

		//create vertex buffer
		m_VertexBuffer = std::make_unique<VertexBuffer>(positions, 8 * 4 * 6 * sizeof(float)); //8 values, 4, points, 6 faces

		//set vertex buffer to array
		VertexBufferLayout layout;
		layout.Push<float>(3);
		layout.Push<float>(3);
		layout.Push<float>(2);
		m_VAO->AddBuffer(*m_VertexBuffer, layout);

		//create index buffer
		m_IndexBuffer = std::make_unique<IndexBuffer>(indicies, 6 * 6);

		//create Vertex and Fragment source
		m_Shader = std::make_unique<Shader>("res/shaders/Advanced.shader");
		m_Shader->Bind();

		//SetTexture
		m_Texture = std::make_unique<Texture>("res/textures/Room.png");
		m_Shader->SetUniform1i("u_Texture", 0);

		//Set Camera
		m_Camera = std::make_unique<Camera>(glm::vec3(0.0f, 0.0f, 3.0f), glm::vec3(0.0f, 0.0f, -3.0f), glm::vec3(0.0f, 1.0f, 0.0f));

		//Set mouse to disabled
		//glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

		//Set callbacks
		//glfwSetCursorPosCallback(window, MouseCallback);
		//glfwSetScrollCallback(window, scroll_callback);
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
		GLCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));

		Renderer renderer;

		m_Texture->Bind();

		//process user input

		{
			m_Shader->Bind();

			//Move Camera
			m_Camera->SetYawPitch(m_YawPitch.x - 90.0f, m_YawPitch.y);
			m_Proj = glm::perspective(glm::radians(m_FOV), 960.0f / 540.0f, 0.1f, 100.0f);

			for (unsigned int i = 0; i < 10; i++)
			{
				//create model matrix
				glm::mat4 model = glm::mat4(1.0f);

				model = glm::translate(model, m_TranslationA);
				model = glm::translate(model, cubePositions[i]);
				model = glm::rotate(model, (float) glfwGetTime() * glm::radians(20.0f + i * 5.0f), glm::vec3(0.5f, 1.0f, 0.0f));

				m_View = m_Camera->viewMatrix;

				//construt model view projection
				glm::mat4 mvp = m_Proj * m_View * model;

				m_Shader->SetUniformMat4f("u_MVP", mvp);

				//draw texture
				renderer.Draw(*m_VAO, *m_IndexBuffer, *m_Shader);
			}

			m_Shader->UnBind();
		}

		m_Texture->UnBind();
	}

	void TestAdvancedTextures::MouseCallback(GLFWwindow* window, double xpos, double ypos)
	{
		if (firstMouse) // this bool variable is initially set to true
		{
			lastX = xpos;
			lastY = ypos;
			firstMouse = false;
		}

		double xOffset = xpos - lastX;
		double yOffset = lastY - ypos;
		lastX = xpos;
		lastY = ypos;

		double sensitivity = 0.05f;
		xOffset *= sensitivity;
		yOffset *= sensitivity;

		m_Camera->UpdateYawPitch((float) xOffset, (float) yOffset);
	}

	void TestAdvancedTextures::ScrollCallback(GLFWwindow* window, double xoffset, double yoffset)
	{
		if (m_FOV >= 1.0f && m_FOV <= 45.0f)
			m_FOV -= yoffset;
		if (m_FOV <= 1.0f)
			m_FOV = 1.0f;
		if (m_FOV >= 45.0f)
			m_FOV = 45.0f;

		m_Proj = glm::perspective(glm::radians(m_FOV), 960.0f / 540.0f, 0.1f, 100.0f);
	}

	void TestAdvancedTextures::ProcessInput(GLFWwindow* window, float deltaTime)
	{
		float cameraSpeed = m_Speed * deltaTime; // adjust accordingly
		if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
			m_Camera->Forward(cameraSpeed);
		if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
			m_Camera->BackWard(cameraSpeed);
		if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
			m_Camera->Up(cameraSpeed);
		if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
			m_Camera->Down(cameraSpeed);
		if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
			m_Camera->Left(cameraSpeed);
		if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
			m_Camera->Right(cameraSpeed);
	}

	void TestAdvancedTextures::OnImGuiRender()
	{
		ImGui::Text("Welcome to the Advanced Test Enviroment. You can explore the enviroment using WASD to move and QE to zoom in and out.");

		//translates imageA
		ImGui::SliderFloat2("Translate Models", &m_TranslationA.x, -2.0f, 2.0f);
		ImGui::SliderFloat2("Yaw Pitch", &m_YawPitch.x, -89.0f, 89.0f);
		ImGui::SliderFloat("FOV", &m_FOV, 1.0f, 89.0f);
		ImGui::SliderFloat("Speed", &m_Speed, 0.5f, 5.0f);
		//displays framerate
		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
	}
}