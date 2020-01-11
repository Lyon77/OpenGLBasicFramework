#include "TestCubeMaps.h"

#include <iostream>

#include "Renderer.h"
#include "imgui/imgui.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

namespace test {

	TestCubeMaps::TestCubeMaps()
		: m_Proj(glm::perspective(glm::radians(45.0f), 960.0f / 540.0f, 0.1f, 100.0f)), m_View(glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, -3.0))), //glm::perspective(glm::radians(45.0f), 960.0f / 540.0f, 0.1f, 500.0f)
		m_CubePos(0, 0, 0), m_FOV(45.0f), m_YawPitch(glm::vec2(0.0f, 0.0f)), m_Speed(2.5f),
		m_Refractive(false), m_Ratio(1.52f)
	{

		//Define Triangle
		// FIX ME: try to get the positions to be from 960 by 540
		float positions[] = {
			//position             //normal          

			//Back
			-0.5f, -0.5f, -0.5f,   0.0f,  0.0f, -1.0f,
			 0.5f, -0.5f, -0.5f,   0.0f,  0.0f, -1.0f,
			 0.5f,  0.5f, -0.5f,   0.0f,  0.0f, -1.0f,
			-0.5f,  0.5f, -0.5f,   0.0f,  0.0f, -1.0f,

			//Front
			-0.5f, -0.5f,  0.5f,   0.0f,  0.0f,  1.0f,
			 0.5f, -0.5f,  0.5f,   0.0f,  0.0f,  1.0f,
			 0.5f,  0.5f,  0.5f,   0.0f,  0.0f,  1.0f,
			-0.5f,  0.5f,  0.5f,   0.0f,  0.0f,  1.0f,

			//Left
			-0.5f, -0.5f,  0.5f,  -1.0f,  0.0f,  0.0f,
			-0.5f, -0.5f, -0.5f,  -1.0f,  0.0f,  0.0f,
			-0.5f,  0.5f, -0.5f,  -1.0f,  0.0f,  0.0f,
			-0.5f,  0.5f,  0.5f,  -1.0f,  0.0f,  0.0f,

			//Right
			 0.5f, -0.5f,  0.5f,   1.0f,  0.0f,  0.0f,
			 0.5f, -0.5f, -0.5f,   1.0f,  0.0f,  0.0f,
			 0.5f,  0.5f, -0.5f,   1.0f,  0.0f,  0.0f,
			 0.5f,  0.5f,  0.5f,   1.0f,  0.0f,  0.0f,

			//Top
			 0.5f,  0.5f,  0.5f,   0.0f,  1.0f,  0.0f,
			 0.5f,  0.5f, -0.5f,   0.0f,  1.0f,  0.0f,
			-0.5f,  0.5f, -0.5f,   0.0f,  1.0f,  0.0f,
			-0.5f,  0.5f,  0.5f,   0.0f,  1.0f,  0.0f,

			//Bottom			
			 0.5f, -0.5f,  0.5f,   0.0f, -1.0f,  0.0f,
			 0.5f, -0.5f, -0.5f,   0.0f, -1.0f,  0.0f,
			-0.5f, -0.5f, -0.5f,   0.0f, -1.0f,  0.0f,
			-0.5f, -0.5f,  0.5f,   0.0f, -1.0f,  0.0f
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

		float skyboxVertices[] = {
			// positions 
			
			//Back
			-1.0f,  1.0f, -1.0f,
			-1.0f, -1.0f, -1.0f,
			 1.0f, -1.0f, -1.0f,
			 1.0f,  1.0f, -1.0f,

			 //Left
			-1.0f, -1.0f,  1.0f,
			-1.0f, -1.0f, -1.0f,
			-1.0f,  1.0f, -1.0f,
			-1.0f,  1.0f,  1.0f,

			//Right
			 1.0f, -1.0f, -1.0f,
			 1.0f, -1.0f,  1.0f,
			 1.0f,  1.0f,  1.0f,
			 1.0f,  1.0f, -1.0f,

			//Front
			-1.0f, -1.0f,  1.0f,
			-1.0f,  1.0f,  1.0f,
			 1.0f,  1.0f,  1.0f,
			 1.0f, -1.0f,  1.0f,

			//Top
			-1.0f,  1.0f, -1.0f,
			 1.0f,  1.0f, -1.0f,
			 1.0f,  1.0f,  1.0f,
			-1.0f,  1.0f,  1.0f,

			//Bottom
			-1.0f, -1.0f, -1.0f,
			-1.0f, -1.0f,  1.0f,
			 1.0f, -1.0f,  1.0f,
			 1.0f, -1.0f, -1.0f
		};

		unsigned int skyBoxIndicies[] = {
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

		std::vector<std::string> faces
		{
			"res/textures/skybox/right.jpg",
			"res/textures/skybox/left.jpg",
			"res/textures/skybox/top.jpg",
			"res/textures/skybox/bottom.jpg",
			"res/textures/skybox/front.jpg",
			"res/textures/skybox/back.jpg"
		};

		//Face Culling
		GLCall(glDisable(GL_CULL_FACE));
		
		//Depth Testing
		GLCall(glEnable(GL_DEPTH_TEST));

		//Stencil Testing
		GLCall(glEnable(GL_STENCIL_TEST));
		GLCall(glStencilFunc(GL_ALWAYS, 1, 0xFF));
		GLCall(glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE));

		//Blending for transparent bits in the texture
		GLCall(glEnable(GL_BLEND));
		GLCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));

		//create vertex array
		m_VAO = std::make_unique<VertexArray>();

		//create vertex buffer
		m_VertexBuffer = std::make_unique<VertexBuffer>(positions, 6 * 4 * 6 * sizeof(float)); //8 values, 4, points, 6 faces

		//set vertex buffer to array
		VertexBufferLayout layout;
		layout.Push<float>(3);
		layout.Push<float>(3);
		m_VAO->AddBuffer(*m_VertexBuffer, layout);

		//create index buffer
		m_IndexBuffer = std::make_unique<IndexBuffer>(indicies, 6 * 6);

		//------------------------ SKYBOX --------------------------

		//create vertex array
		m_SkyBoxVAO = std::make_unique<VertexArray>();

		//create vertex buffer
		m_SkyBoxVertexBuffer = std::make_unique<VertexBuffer>(skyboxVertices, 3 * 4 * 6 * sizeof(float)); //3 values, 4, points, 6 faces

		//set vertex buffer to array
		VertexBufferLayout skyBoxLayout;
		skyBoxLayout.Push<float>(3);
		m_SkyBoxVAO->AddBuffer(*m_SkyBoxVertexBuffer, skyBoxLayout);

		//create index buffer
		m_SkyBoxIndexBuffer = std::make_unique<IndexBuffer>(skyBoxIndicies, 6 * 6);

		//-----------------------------------------------------------

		//create Vertex and Fragment source
		m_Shader = std::make_unique<Shader>("res/shaders/Reflective.shader");
		m_Shader->Bind();
		m_Shader->SetUniform1i("u_SkyBox", 0);
		m_Shader->UnBind();

		m_RefractiveShader = std::make_unique<Shader>("res/shaders/Refractive.shader");
		m_RefractiveShader->Bind();
		m_RefractiveShader->SetUniform1i("u_SkyBox", 0);
		m_RefractiveShader->UnBind();

		m_SkyBoxShader = std::make_unique<Shader>("res/shaders/CubeMap.shader");
		m_SkyBoxShader->Bind();
		m_SkyBoxShader->SetUniform1i("u_SkyBox", 0);
		m_SkyBoxShader->UnBind();

		//Set Camera
		m_Camera = std::make_unique<Camera>(glm::vec3(0.0f, 0.0f, 3.0f), glm::vec3(0.0f, 0.0f, -3.0f), glm::vec3(0.0f, 1.0f, 0.0f));

		//Load SkyBox
		m_SkyBoxTexture = std::make_unique<CubeMap>(faces);
	}

	TestCubeMaps::~TestCubeMaps()
	{
	}

	void TestCubeMaps::OnUpdate(float deltaTime)
	{
	}

	void TestCubeMaps::OnRender()
	{
		GLCall(glClearColor(0.0f, 0.0f, 0.0f, 1.0f));
		GLCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT));

		Renderer renderer;

		//process user input

		//Move Camera
		m_Camera->SetYawPitch(m_YawPitch.x - 90.0f, m_YawPitch.y);
		m_Proj = glm::perspective(glm::radians(m_FOV), 960.0f / 540.0f, 0.1f, 100.0f);

		// Load the box
		{
			if (!m_Refractive)
			{
				m_Shader->Bind();

				//Bind Texture
				m_SkyBoxTexture->Bind();

				//m_Shader->SetUniform1f("u_Ratio", m_Ratio);
				m_Shader->SetUniform3f("u_CameraPos", m_Camera->CameraPosition().x, m_Camera->CameraPosition().y, m_Camera->CameraPosition().z);


				for (unsigned int i = 0; i < 10; i++)
				{
					//create model matrix
					glm::mat4 model = glm::mat4(1.0f);

					model = glm::translate(model, m_CubePos + cubePositions[i]);
					model = glm::rotate(model, (float)glfwGetTime() * glm::radians(20.0f + i * 5.0f), glm::vec3(0.5f, 1.0f, 0.0f));

					m_View = m_Camera->viewMatrix;

					//construt model view projection
					glm::mat4 mvp = m_Proj * m_View * model;

					m_Shader->SetUniformMat4f("u_MVP", mvp);
					m_Shader->SetUniformMat4f("u_Model", model);

					//draw texture
					renderer.Draw(*m_VAO, *m_IndexBuffer, *m_Shader);
				}

				m_SkyBoxTexture->UnBind();

				m_Shader->UnBind();
			} 
			else
			{
				m_RefractiveShader->Bind();

				//Bind Texture
				m_SkyBoxTexture->Bind();

				m_RefractiveShader->SetUniform1f("u_Ratio", m_Ratio);
				m_RefractiveShader->SetUniform3f("u_CameraPos", m_Camera->CameraPosition().x, m_Camera->CameraPosition().y, m_Camera->CameraPosition().z);


				for (unsigned int i = 0; i < 10; i++)
				{
					//create model matrix
					glm::mat4 model = glm::mat4(1.0f);

					model = glm::translate(model, m_CubePos + cubePositions[i]);
					model = glm::rotate(model, (float)glfwGetTime() * glm::radians(20.0f + i * 5.0f), glm::vec3(0.5f, 1.0f, 0.0f));

					m_View = m_Camera->viewMatrix;

					//construt model view projection
					glm::mat4 mvp = m_Proj * m_View * model;

					m_RefractiveShader->SetUniformMat4f("u_MVP", mvp);
					m_RefractiveShader->SetUniformMat4f("u_Model", model);

					//draw texture
					renderer.Draw(*m_VAO, *m_IndexBuffer, *m_RefractiveShader);
				}

				m_SkyBoxTexture->UnBind();

				m_RefractiveShader->UnBind();
			}
			
		}

		//Draw Skybox
		{
			glDepthFunc(GL_LEQUAL);
			m_SkyBoxShader->Bind();

			m_View = glm::mat4(glm::mat3(m_Camera->viewMatrix));

			glm::mat4 mvp = m_Proj * m_View;
			m_SkyBoxShader->SetUniformMat4f("u_MVP", mvp);

			//draw
			m_SkyBoxTexture->Bind();
			renderer.Draw(*m_SkyBoxVAO, *m_SkyBoxIndexBuffer, *m_SkyBoxShader);

			m_SkyBoxTexture->UnBind();
			m_SkyBoxShader->UnBind();
			glDepthFunc(GL_LESS);
		}
	}

	void TestCubeMaps::MouseCallback(GLFWwindow* window, double xpos, double ypos)
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

	void TestCubeMaps::ScrollCallback(GLFWwindow* window, double xoffset, double yoffset)
	{
		if (m_FOV >= 1.0f && m_FOV <= 45.0f)
			m_FOV -= yoffset;
		if (m_FOV <= 1.0f)
			m_FOV = 1.0f;
		if (m_FOV >= 45.0f)
			m_FOV = 45.0f;

		m_Proj = glm::perspective(glm::radians(m_FOV), 960.0f / 540.0f, 0.1f, 100.0f);
	}

	void TestCubeMaps::ProcessInput(GLFWwindow* window, float deltaTime)
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

	void TestCubeMaps::OnImGuiRender()
	{
		ImGui::Text("Welcome to the Depth, Stencil, and Blending Test Enviroment.");

		ImGui::Checkbox("Refractive", &m_Refractive);

		if (ImGui::CollapsingHeader("Camera Options")) {
			ImGui::SliderFloat2("Yaw Pitch", &m_YawPitch.x, -180.0f, 180.0f);
			ImGui::SliderFloat("FOV", &m_FOV, 1.0f, 89.0f);
			ImGui::SliderFloat("Speed", &m_Speed, 0.5f, 5.0f);
		}

		if (ImGui::CollapsingHeader("Cube Options")) {
			ImGui::SliderFloat3("Translate Cube", &m_CubePos.x, -5.0f, 5.0f);
			if (m_Refractive)
				ImGui::SliderFloat("Cube Refration", &m_Ratio, 1.0f, 3.0f);
		}

		//displays framerate
		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
	}
}