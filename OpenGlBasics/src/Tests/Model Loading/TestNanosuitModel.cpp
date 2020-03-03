#include "TestNanosuitModel.h"

#include <iostream>
#include <filesystem>

#include "Renderer.h"
#include "imgui/imgui.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

namespace test {
	TestNanosuitModel::TestNanosuitModel()
		: m_Proj(glm::perspective(glm::radians(45.0f), 960.0f / 540.0f, 0.1f, 100.0f)), m_View(glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, -3.0))),
		m_FOV(45.0f), m_YawPitch(glm::vec2(0.0f, 0.0f)), m_Speed(2.5f)
	{
		//Depth Testing
		GLCall(glEnable(GL_DEPTH_TEST));

		//Blending for transparent bits in the texture
		GLCall(glDisable(GL_BLEND));

		//create Vertex and Fragment source
		m_Shader = std::make_unique<Shader>("res/shaders/Model.shader");

		m_Model = std::make_unique<Model>("res/models/nanosuit/nanosuit.obj");
		
		//Set Camera
		m_Camera = std::make_unique<Camera>(glm::vec3(0.0f, 5.0f, 10.0f), glm::vec3(0.0f, 5.0f, -3.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	}
	TestNanosuitModel::~TestNanosuitModel()
	{
	}
	void TestNanosuitModel::OnUpdate(float deltaTime)
	{
	}
	void TestNanosuitModel::OnRender()
	{
		GLCall(glClearColor(0.0f, 0.0f, 0.0f, 1.0f));
		GLCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT));

		{
			m_Shader->Bind();

			//Move Camera
			m_Camera->SetYawPitch(m_YawPitch.x - 90.0f, m_YawPitch.y);
			m_Proj = glm::perspective(glm::radians(m_FOV), 960.0f / 540.0f, 0.1f, 100.0f);

			for (unsigned int i = 0; i < 10; i++)
			{
				//create model matrix
				glm::mat4 model = glm::mat4(1.0f);

				m_View = m_Camera->viewMatrix;

				//construt model view projection
				glm::mat4 mvp = m_Proj * m_View * model;

				m_Shader->SetUniformMat4f("u_MVP", mvp);

				//draw texture
				m_Model->Draw(m_Shader.get());
			}

			m_Shader->UnBind();
		}
	}

	void TestNanosuitModel::MouseCallback(GLFWwindow* window, double xpos, double ypos)
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

		m_Camera->UpdateYawPitch((float)xOffset, (float)yOffset);
	}

	void TestNanosuitModel::ScrollCallback(GLFWwindow* window, double xoffset, double yoffset)
	{
		if (m_FOV >= 1.0f && m_FOV <= 45.0f)
			m_FOV -= yoffset;
		if (m_FOV <= 1.0f)
			m_FOV = 1.0f;
		if (m_FOV >= 45.0f)
			m_FOV = 45.0f;

		m_Proj = glm::perspective(glm::radians(m_FOV), 960.0f / 540.0f, 0.1f, 100.0f);
	}

	void TestNanosuitModel::ProcessInput(GLFWwindow* window, float deltaTime)
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

	void TestNanosuitModel::OnImGuiRender()
	{
		ImGui::TextWrapped("Welcome to the Advanced Test Enviroment. You can explore the enviroment using WASD to move and QE to zoom in and out.");

		//translates imageA
		ImGui::SliderFloat2("Yaw Pitch", &m_YawPitch.x, -89.0f, 89.0f);
		ImGui::SliderFloat("FOV", &m_FOV, 1.0f, 89.0f);
		ImGui::SliderFloat("Speed", &m_Speed, 0.5f, 5.0f);

		//displays framerate
		ImGui::TextWrapped("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
	}
}