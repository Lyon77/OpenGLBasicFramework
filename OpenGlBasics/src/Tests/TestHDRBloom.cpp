#include "TestHDRBloom.h"

#include <iostream>

#include "Renderer.h"
#include "imgui/imgui.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

namespace test {

	TestHDRBloom::TestHDRBloom()
		: m_Proj(glm::perspective(glm::radians(45.0f), 960.0f / 540.0f, 0.1f, 100.0f)), m_View(glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, -3.0))), //glm::perspective(glm::radians(45.0f), 960.0f / 540.0f, 0.1f, 500.0f)
		m_LampPos(glm::vec3(0.0f, 0.0f, 0.1f)),
		m_CubePos(0, 0, 0), m_FOV(45.0f), m_YawPitch(glm::vec2(0.0f, 0.0f)), m_Speed(2.5f),
		m_CubeColor(glm::vec3(1.0f, 1.0f, 1.0f)),
		m_SpecularPower(5.0f),
		m_AttenuationCheckbox(true),
		m_Exposure(1.0f)
	{

		//Define Triangle
		// FIX ME: try to get the positions to be from 960 by 540
		float positions[] = {
			//position             //normal          	  //texture
			-0.5f, -0.5f,  0.0f,   0.0f,  0.0f,  1.0f,	  0.0f, 0.0f,
			 0.5f, -0.5f,  0.0f,   0.0f,  0.0f,  1.0f,	  1.0f, 0.0f,
			 0.5f,  0.5f,  0.0f,   0.0f,  0.0f,  1.0f,	  1.0f, 1.0f,
			-0.5f,  0.5f,  0.0f,   0.0f,  0.0f,  1.0f,	  0.0f, 1.0f
		};

		unsigned int indicies[] = {
			0, 2, 1,
			2, 0, 3
		};

		float frameScreenPositions[] = {
			//  position      texCoords
				-1.0, 1.0,    0.0f, 1.0f,
				-1.0,-1.0,    0.0f, 0.0f,
				 1.0,-1.0,    1.0f, 0.0f,
				 1.0, 1.0,    1.0f, 1.0f
		};

		unsigned int frameScreenIndicies[] = {
			0, 1, 2,
			2, 3, 0
		};

		//Depth Testing
		GLCall(glEnable(GL_DEPTH_TEST));

		//Blending for transparent bits in the texture
		GLCall(glEnable(GL_BLEND));
		GLCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));

		//create vertex array
		m_VAO = std::make_unique<VertexArray>();
		m_FrameVAO = std::make_unique<VertexArray>();


		//create vertex buffer
		m_VertexBuffer = std::make_unique<VertexBuffer>(positions, 8 * 4 * 1 * sizeof(float)); //3 values, 4, points, 6 faces
		m_FrameVertexBuffer = std::make_unique<VertexBuffer>(frameScreenPositions, 4 * 4 * 1 * sizeof(float));


		//set vertex buffer to array
		VertexBufferLayout layout;
		layout.Push<float>(3);
		layout.Push<float>(3);
		layout.Push<float>(2);
		m_VAO->AddBuffer(*m_VertexBuffer, layout);

		VertexBufferLayout frameLayout;
		frameLayout.Push<float>(2);
		frameLayout.Push<float>(2);
		m_FrameVAO->AddBuffer(*m_FrameVertexBuffer, frameLayout);

		//create index buffer
		m_IndexBuffer = std::make_unique<IndexBuffer>(indicies, 6 * 1);
		m_FrameIndexBuffer = std::make_unique<IndexBuffer>(frameScreenIndicies, 6 * 1);

		//create Vertex and Fragment source
		m_Shader = std::make_unique<Shader>("res/shaders/BlinnPhong.shader");

		m_LampShader = std::make_unique<Shader>("res/shaders/Lamp.shader");

		m_Shader->Bind();

		//Load Texture
		m_TextureDiffuse = std::make_unique<Texture>("res/textures/Box_diffuse.png");
		m_TextureSpecular = std::make_unique<Texture>("res/textures/Box_specular.png");

		m_Shader->SetUniform1i("u_Gamma", false);
		m_Shader->SetUniform1i("u_Material.diffuse", 0);
		m_Shader->SetUniform1i("u_Material.specular", 1);

		//Set up FrameBuffer
		m_FrameShader = std::make_unique<Shader>("res/shaders/HDR.shader");
		m_FrameBuffer = std::make_unique<FrameBuffer>();
		m_FrameBuffer->AddColorFPAttachment();

		//Set Camera
		m_Camera = std::make_unique<Camera>(glm::vec3(0.0f, 0.0f, 4.0f), glm::vec3(0.0f, 0.0f, -3.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	}

	TestHDRBloom::~TestHDRBloom()
	{
	}

	void TestHDRBloom::OnUpdate(float deltaTime)
	{
	}

	void TestHDRBloom::OnRender()
	{
		//Set FrameBuffer to the created one
		m_FrameBuffer->Bind();

		GLCall(glClearColor(0.0f, 0.0f, 0.0f, 1.0f));
		GLCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT));

		Renderer renderer;

		//m_Texture->Bind();

		//process user input

		// Load the box
		{
			m_Shader->Bind();

			m_Shader->SetUniform1i("u_Att", m_AttenuationCheckbox);

			m_TextureDiffuse->Bind();
			m_TextureSpecular->Bind(1);

			m_Shader->SetUniform3f("u_ObjectColor", m_CubeColor.x, m_CubeColor.y, m_CubeColor.z);

			m_Shader->SetUniform3f("u_ViewPos", m_Camera->CameraPosition().x, m_Camera->CameraPosition().y, m_Camera->CameraPosition().z);

			m_Shader->SetUniform1i("u_NumPointLights", 3);

			m_Shader->SetUniform3f("u_PointLight[0].position", m_LampPos.x, m_LampPos.y, m_LampPos.z);
			m_Shader->SetUniform3f("u_PointLight[0].ambient", 3.0f, 3.0f, 3.0f);
			m_Shader->SetUniform3f("u_PointLight[0].diffuse", 3.0f, 3.0f, 3.0f);
			m_Shader->SetUniform3f("u_PointLight[0].specular", 3.0f, 3.0f, 3.0f);
			m_Shader->SetUniform1f("u_PointLight[0].constant", 1.0f);
			m_Shader->SetUniform1f("u_PointLight[0].linear", 0.09f);
			m_Shader->SetUniform1f("u_PointLight[0].quadratic", 0.032f);

			m_Shader->SetUniform3f("u_PointLight[1].position", m_LampPos.x + 0.1f, m_LampPos.y -0.3f, m_LampPos.z + 3.0f);
			m_Shader->SetUniform3f("u_PointLight[1].ambient", 0.0f, 0.0f, 0.1f);
			m_Shader->SetUniform3f("u_PointLight[1].diffuse", 0.0f, 0.0f, 0.1f);
			m_Shader->SetUniform3f("u_PointLight[1].specular", 0.0f, 0.0f, 0.1f);
			m_Shader->SetUniform1f("u_PointLight[1].constant", 1.0f);
			m_Shader->SetUniform1f("u_PointLight[1].linear", 0.09f);
			m_Shader->SetUniform1f("u_PointLight[1].quadratic", 0.032f);

			m_Shader->SetUniform3f("u_PointLight[2].position", m_LampPos.x - 0.3f, m_LampPos.y - 0.1f, m_LampPos.z + 3.0f);
			m_Shader->SetUniform3f("u_PointLight[2].ambient", 0.1f, 0.0f, 0.0f);
			m_Shader->SetUniform3f("u_PointLight[2].diffuse", 0.1f, 0.0f, 0.0f);
			m_Shader->SetUniform3f("u_PointLight[2].specular", 0.1f, 0.0f, 0.0f);
			m_Shader->SetUniform1f("u_PointLight[2].constant", 1.0f);
			m_Shader->SetUniform1f("u_PointLight[2].linear", 0.09f);
			m_Shader->SetUniform1f("u_PointLight[2].quadratic", 0.032f);

			m_Shader->SetUniform1f("u_Material.shininess", m_SpecularPower);

			//Move Camera
			m_Camera->SetYawPitch(m_YawPitch.x - 90.0f, m_YawPitch.y);
			m_Proj = glm::perspective(glm::radians(m_FOV), 960.0f / 540.0f, 0.1f, 100.0f);

			for (unsigned int i = 0; i < 1 + 4 * 5; i++)
			{
				//create model matrix
				glm::mat4 model = glm::mat4(1.0f);

				model = glm::translate(model, m_CubePos + cubePositions[i]);
				model = glm::rotate(model, glm::radians(cubeRotations[i].x), glm::vec3(1.0, 0.0, 0.0));
				model = glm::rotate(model, glm::radians(cubeRotations[i].y), glm::vec3(0.0, 1.0, 0.0));
				m_View = m_Camera->viewMatrix;

				//construt model view projection
				glm::mat4 mvp = m_Proj * m_View * model;

				m_Shader->SetUniformMat4f("u_MVP", mvp);
				m_Shader->SetUniformMat4f("u_Model", model);

				//draw texture
				renderer.Draw(*m_VAO, *m_IndexBuffer, *m_Shader);
			}

			m_TextureDiffuse->UnBind();
			m_Shader->UnBind();
		}

		// Load the Lamp
		{
			m_LampShader->Bind();

			//Move Camera
			m_Camera->SetYawPitch(m_YawPitch.x - 90.0f, m_YawPitch.y);
			m_Proj = glm::perspective(glm::radians(m_FOV), 960.0f / 540.0f, 0.1f, 100.0f);

			{
				//create model matrix
				glm::mat4 model = glm::mat4(1.0f);

				model = glm::translate(model, m_LampPos);
				model = glm::scale(model, glm::vec3(0.05f));

				m_View = m_Camera->viewMatrix;

				//construt model view projection
				glm::mat4 mvp = m_Proj * m_View * model;

				m_LampShader->SetUniformMat4f("u_MVP", mvp);
				m_LampShader->SetUniform4f("u_Color", 3.0f, 3.0f, 3.0f, 3.0f);

				//draw texture
				renderer.Draw(*m_VAO, *m_IndexBuffer, *m_LampShader);
			}

			{
				//create model matrix
				glm::mat4 model = glm::mat4(1.0f);

				model = glm::translate(model, m_LampPos);
				model = glm::translate(model, glm::vec3(0.1f, -0.3f, 3.0f));
				model = glm::scale(model, glm::vec3(0.05f));

				m_View = m_Camera->viewMatrix;

				//construt model view projection
				glm::mat4 mvp = m_Proj * m_View * model;

				m_LampShader->SetUniformMat4f("u_MVP", mvp);
				m_LampShader->SetUniform4f("u_Color", 0.0f, 0.0f, 0.1f, 1.0f);


				//draw texture
				renderer.Draw(*m_VAO, *m_IndexBuffer, *m_LampShader);
			}

			{
				//create model matrix
				glm::mat4 model = glm::mat4(1.0f);

				model = glm::translate(model, m_LampPos);
				model = glm::translate(model, glm::vec3(-0.3f, -0.1f, 3.0f));
				model = glm::scale(model, glm::vec3(0.05f));

				m_View = m_Camera->viewMatrix;

				//construt model view projection
				glm::mat4 mvp = m_Proj * m_View * model;

				m_LampShader->SetUniformMat4f("u_MVP", mvp);
				m_LampShader->SetUniform4f("u_Color", 0.1f, 0.0f, 0.0f, 1.0f);

				//draw texture
				renderer.Draw(*m_VAO, *m_IndexBuffer, *m_LampShader);
			}
		}


		m_FrameBuffer->UnBind();

		GLCall(glClearColor(1.0f, 1.0f, 1.0f, 1.0f));
		GLCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));

		{
			m_FrameShader->Bind();
			m_FrameBuffer->BindColorTexture();

			m_FrameShader->SetUniform1f("u_Exposure", m_Exposure);

			renderer.Draw(*m_FrameVAO, *m_FrameIndexBuffer, *m_FrameShader);
		}
	}

	void TestHDRBloom::MouseCallback(GLFWwindow* window, double xpos, double ypos)
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

	void TestHDRBloom::ScrollCallback(GLFWwindow* window, double xoffset, double yoffset)
	{
		if (m_FOV >= 1.0f && m_FOV <= 45.0f)
			m_FOV -= yoffset;
		if (m_FOV <= 1.0f)
			m_FOV = 1.0f;
		if (m_FOV >= 45.0f)
			m_FOV = 45.0f;

		m_Proj = glm::perspective(glm::radians(m_FOV), 960.0f / 540.0f, 0.1f, 100.0f);
	}

	void TestHDRBloom::ProcessInput(GLFWwindow* window, float deltaTime)
	{
		float cameraSpeed = m_Speed * deltaTime; // adjust accordingly
		if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS && m_Camera->CameraPosition().z > 1.0f)
			m_Camera->Forward(cameraSpeed);
		if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS && m_Camera->CameraPosition().z < 5.0f)
			m_Camera->BackWard(cameraSpeed);
		if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS && m_Camera->CameraPosition().y < 0.25f)
			m_Camera->Up(cameraSpeed);
		if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS && m_Camera->CameraPosition().y > -0.25f)
			m_Camera->Down(cameraSpeed);
		if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS && m_Camera->CameraPosition().x > -0.25f)
			m_Camera->Left(cameraSpeed);
		if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS && m_Camera->CameraPosition().x < 0.25f)
			m_Camera->Right(cameraSpeed);
	}

	void TestHDRBloom::OnImGuiRender()
	{
		ImGui::Text("Welcome to the HDR and Bloom Test Enviroment. Use WASD to move around and QE to zoom in and out. There are more setting options below.");

		ImGui::SliderFloat("Translate Lamp", &m_LampPos.z, 0.1f, 3.0f);
		ImGui::SliderFloat("Exposure", &m_Exposure, 0.1f, 5.0f);

		//displays framerate
		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
	}
}