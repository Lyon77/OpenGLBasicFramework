#include "TestPBR.h"

#include <iostream>

#include "Renderer.h"
#include "imgui/imgui.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

namespace test {

	TestPBR::TestPBR()
		: m_Proj(glm::perspective(glm::radians(45.0f), 960.0f / 540.0f, 0.1f, 100.0f)), m_View(glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, -3.0))), //glm::perspective(glm::radians(45.0f), 960.0f / 540.0f, 0.1f, 500.0f)
		m_FOV(45.0f), m_YawPitch(glm::vec2(0.0f, 0.0f)), m_Speed(10.0f),
		m_TexturedSphere(false), m_Albedo("Plastic"),
		m_LampPos(glm::vec3(0.2f, 0.3f, 0.5f))
	{
		// -------------------Create Sphere------------------

		std::vector<glm::vec3> positions;
		std::vector<glm::vec2> uv;
		std::vector<glm::vec3> normals;
		std::vector<unsigned int> indices;

		const unsigned int X_SEGMENTS = 64;
		const unsigned int Y_SEGMENTS = 64;
		const float PI = 3.14159265359;

		for (unsigned int y = 0; y <= Y_SEGMENTS; ++y)
		{
			for (unsigned int x = 0; x <= X_SEGMENTS; ++x)
			{
				float xSegment = (float)x / (float)X_SEGMENTS;
				float ySegment = (float)y / (float)Y_SEGMENTS;

				float xPos = std::cos(xSegment * 2.0f * PI) * std::sin(ySegment * PI);
				float yPos = std::cos(ySegment * PI);
				float zPos = std::sin(xSegment * 2.0f * PI) * std::sin(ySegment * PI);

				positions.push_back(glm::vec3(xPos, yPos, zPos));
				uv.push_back(glm::vec2(xSegment, ySegment));
				normals.push_back(glm::vec3(xPos, yPos, zPos));
			}
		}

		bool oddRow = false;
		for (unsigned int y = 0; y <= Y_SEGMENTS; ++y)
		{
			for (int x = 0; x <= X_SEGMENTS; ++x)
			{
				if (!oddRow)
				{
					indices.push_back(y * (X_SEGMENTS + 1) + x);
					indices.push_back((y + 1) * (X_SEGMENTS + 1) + x);
				}
				else
				{
					indices.push_back((y + 1) * (X_SEGMENTS + 1) + x);
					indices.push_back(y * (X_SEGMENTS + 1) + x);
				}
			}
			oddRow = !oddRow;
		}

		std::vector<float> data;
		for (int i = 0; i < positions.size(); ++i)
		{
			data.push_back(positions[i].x);
			data.push_back(positions[i].y);
			data.push_back(positions[i].z);
			if (uv.size() > 0)
			{
				data.push_back(uv[i].x);
				data.push_back(uv[i].y);
			}
			if (normals.size() > 0)
			{
				data.push_back(normals[i].x);
				data.push_back(normals[i].y);
				data.push_back(normals[i].z);
			}
		}

		// --------------------------------------------------

		//Depth Testing
		GLCall(glEnable(GL_DEPTH_TEST));

		//Blending for transparent bits in the texture
		GLCall(glEnable(GL_BLEND));
		GLCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));

		m_VAO = std::make_unique<VertexArray>();

		m_VertexBuffer = std::make_unique<VertexBuffer>(&data[0], data.size() * sizeof(float));

		//set vertex buffer to array
		VertexBufferLayout layout;
		layout.Push<float>(3);
		layout.Push<float>(2);
		layout.Push<float>(3);
		m_VAO->AddBuffer(*m_VertexBuffer, layout);

		m_IndexBuffer = std::make_unique<IndexBuffer>(&indices[0], indices.size());

		//create Vertex and Fragment source
		m_Shader = std::make_unique<Shader>("res/shaders/PBR.shader");
		m_Shader->Bind();
		m_Shader->SetUniform1i("u_AlbedoMap", 0);
		m_Shader->SetUniform1i("u_MetallicMap", 1);
		m_Shader->SetUniform1i("u_NormalMap", 2);
		m_Shader->SetUniform1i("u_RoughnessMap", 3);
		m_Shader->UnBind();

		m_LampShader = std::make_unique<Shader>("res/shaders/Lamp.shader");

		//Set Textures
		m_AlbedoTexture = std::make_unique<Texture>("res/textures/rusted_iron/albedo.png");
		m_MetallicTexture = std::make_unique<Texture>("res/textures/rusted_iron/metallic.png");
		m_NormalTexture = std::make_unique<Texture>("res/textures/rusted_iron/normal.png");
		m_RoughnessTexture = std::make_unique<Texture>("res/textures/rusted_iron/roughness.png");

		//Set Camera
		m_Camera = std::make_unique<Camera>(glm::vec3(0.0f, 0.0f, 20.0f), glm::vec3(0.0f, 0.0f, -3.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	}

	TestPBR::~TestPBR()
	{
	}

	void TestPBR::OnUpdate(float deltaTime)
	{
	}

	void TestPBR::OnRender()
	{
		Renderer renderer;
		GLCall(glClearColor(0.0f, 0.0f, 0.0f, 1.0f));
		GLCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT));

		//Move Camera
		m_Camera->SetYawPitch(m_YawPitch.x - 90.0f, m_YawPitch.y);
		m_Proj = glm::perspective(glm::radians(m_FOV), 960.0f / 540.0f, 0.1f, 100.0f);

		// ------Make this section global----------
		int nrRows = 7;
		int nrColumns = 7;
		float spacing = 2.5;

		glm::vec3 lightPositions[] = {
			glm::vec3(-10.0f,  10.0f, 10.0f),
			glm::vec3(10.0f,  10.0f, 10.0f),
			glm::vec3(-10.0f, -10.0f, 10.0f),
			glm::vec3(10.0f, -10.0f, 10.0f)
		};
		glm::vec3 lightColors[] = {
			glm::vec3(300.0f, 300.0f, 300.0f),
			glm::vec3(300.0f, 300.0f, 300.0f),
			glm::vec3(300.0f, 300.0f, 300.0f),
			glm::vec3(300.0f, 300.0f, 300.0f)
		};
		// ----------------------------------------

		{
			m_Shader->Bind();

			//Set Uniforms
			m_Shader->SetUniform3f("u_CameraPos", m_Camera->CameraPosition().x, m_Camera->CameraPosition().y, m_Camera->CameraPosition().z);
			m_Shader->SetUniform1f("u_AO", 1.0f);
			m_Shader->SetUniform1i("u_Textured", m_TexturedSphere);

			glm::vec3 type = AlbedoType();
			m_Shader->SetUniform3f("u_Albedo", type.x, type.y, type.z);

			m_AlbedoTexture->Bind(0);
			m_MetallicTexture->Bind(1);
			m_NormalTexture->Bind(2);
			m_RoughnessTexture->Bind(3);

			glm::mat4 model = glm::mat4(1.0f);
			m_View = m_Camera->viewMatrix;

			// Setup Lights
			for (unsigned int i = 0; i < sizeof(lightPositions) / sizeof(lightPositions[0]); ++i)
			{
				//glm::vec3 newPos = lightPositions[i] + glm::vec3(sin(glfwGetTime() * 5.0) * 5.0, 0.0, 0.0);
				//newPos = lightPositions[i];
				m_Shader->SetUniform3f("u_LightPositions[" + std::to_string(i) + "]", lightPositions[i].x, lightPositions[i].y, lightPositions[i].z);
				m_Shader->SetUniform3f("u_LightColors[" + std::to_string(i) + "]", lightColors[i].x, lightColors[i].y, lightColors[i].z);
			}

			for (int row = 0; row < nrRows; row++) 
			{
				m_Shader->SetUniform1f("u_Metallic", (float)row / (float)nrRows);

				for (int col = 0; col < nrColumns; ++col)
				{
					// we clamp the roughness to 0.025 - 1.0 as perfectly smooth surfaces (roughness of 0.0) tend to look a bit off
					// on direct lighting.
					m_Shader->SetUniform1f("u_Roughness", glm::clamp((float)col / (float)nrColumns, 0.05f, 1.0f));

					// Reset model and recalculate location
					model = glm::mat4(1.0f);
					model = glm::translate(model, glm::vec3(
						(col - (nrColumns / 2)) * spacing,
						(row - (nrRows / 2)) * spacing,
						0.0f
					));

					//construt model view projection
					glm::mat4 mvp = m_Proj * m_View * model;

					m_Shader->SetUniformMat4f("u_MVP", mvp);
					m_Shader->SetUniformMat4f("u_Model", model);

					renderer.DrawTriangleStrips(*m_VAO, *m_IndexBuffer, *m_Shader);
				}
			}

			m_Shader->UnBind();
		}

		// Load the Lamp
		{
			m_LampShader->Bind();


			for (unsigned int i = 0; i < sizeof(lightPositions) / sizeof(lightPositions[0]); ++i)
			{
				//glm::vec3 newPos = lightPositions[i] + glm::vec3(sin(glfwGetTime() * 5.0) * 5.0, 0.0, 0.0);
				//newPos = lightPositions[i];
				
				//create model matrix
				glm::mat4 model = glm::mat4(1.0f);

				model = glm::translate(model, lightPositions[i]);
				model = glm::scale(model, glm::vec3(0.1f));

				m_View = m_Camera->viewMatrix;

				//construt model view projection
				glm::mat4 mvp = m_Proj * m_View * model;

				m_LampShader->SetUniformMat4f("u_MVP", mvp);

				//draw texture
				renderer.Draw(*m_VAO, *m_IndexBuffer, *m_LampShader);
			}

			m_LampShader->UnBind();
		}
	}

	void TestPBR::MouseCallback(GLFWwindow* window, double xpos, double ypos)
	{
		if (firstMouse) // this bool variable is initially set to true
		{
			lastX = xpos;
			lastY = ypos;
			firstMouse = false;
		}

		float xOffset = xpos - lastX;
		float yOffset = lastY - ypos;
		lastX = xpos;
		lastY = ypos;

		float sensitivity = 0.05f;
		xOffset *= sensitivity;
		yOffset *= sensitivity;
		m_Camera->UpdateYawPitch(xOffset, yOffset);
	}

	void TestPBR::ScrollCallback(GLFWwindow* window, double xoffset, double yoffset)
	{
		if (m_FOV >= 1.0f && m_FOV <= 45.0f)
			m_FOV -= yoffset;
		if (m_FOV <= 1.0f)
			m_FOV = 1.0f;
		if (m_FOV >= 45.0f)
			m_FOV = 45.0f;

		m_Proj = glm::perspective(glm::radians(m_FOV), 960.0f / 540.0f, 0.1f, 100.0f);
	}

	glm::vec3 TestPBR::AlbedoType()
	{
		glm::vec3 output = glm::vec3(1.0f);

		if (m_Albedo == "Plastic")
			output = glm::vec3(0.04f, 0.04f, 0.04f);
		if (m_Albedo == "Diamond")
			output = glm::vec3(0.17f, 0.17f, 0.17f);
		if (m_Albedo == "Iron")
			output = glm::vec3(0.56f, 0.57f, 0.58f);
		if (m_Albedo == "Copper")
			output = glm::vec3(0.95f, 0.64f, 0.54f);
		if (m_Albedo == "Gold")
			output = glm::vec3(1.00f, 0.71f, 0.29f);
		if (m_Albedo == "Aluminum")
			output = glm::vec3(0.91f, 0.92f, 0.92f);
		if (m_Albedo == "Silver")
			output = glm::vec3(0.95f, 0.93f, 0.88f);

		return output;
	}

	void TestPBR::ProcessInput(GLFWwindow* window, float deltaTime)
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

	void TestPBR::OnImGuiRender()
	{
		ImGui::TextWrapped("Welcome to the PBR Test Enviroment. The metallic value ranges from 0.0 to 1.0 from top to bottom and roughness from 0.0 to 1.0 left to right. The Rusted Iron texture has all the spheres be the same. The Use WASD to move around and QE to zoom in and out so that you can see the shadows. There are more setting options below.");

		ImGui::Checkbox("Rusted Iron Texture", &m_TexturedSphere);

		std::vector<std::string> items { "Plastic", "Diamond", "Iron", "Copper", "Gold", "Aluminum", "Silver" };
		if (ImGui::BeginCombo("Albedo Color", m_Albedo.c_str())) // The second parameter is the label previewed before opening the combo.
		{
			for (int n = 0; n < items.size(); n++)
			{
				bool is_selected = (m_Albedo == items[n]);
				if (ImGui::Selectable(items[n].c_str(), is_selected))
				{
					m_Albedo = items[n];
					if (is_selected)
						ImGui::SetItemDefaultFocus();   // You may set the initial focus when opening the combo (scrolling + for keyboard navigation support)
				}
			}
			ImGui::EndCombo();
		}

		ImGui::SliderFloat2("Yaw Pitch", &m_YawPitch.x, -89.0f, 89.0f);
		ImGui::SliderFloat("FOV", &m_FOV, 1.0f, 89.0f);
		ImGui::SliderFloat("Speed", &m_Speed, 5.0f, 20.0f);

		//displays framerate
		ImGui::TextWrapped("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
	}
}