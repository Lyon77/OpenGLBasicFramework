#include "TestDepthStencilBlending.h"

#include <iostream>

#include "Renderer.h"
#include "imgui/imgui.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

namespace test {

	TestDepthStencilBlending::TestDepthStencilBlending()
		: m_Proj(glm::perspective(glm::radians(45.0f), 960.0f / 540.0f, 0.1f, 100.0f)), m_View(glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, -3.0))), //glm::perspective(glm::radians(45.0f), 960.0f / 540.0f, 0.1f, 500.0f)
		m_CubePos(0, 0, 0), m_FOV(45.0f), m_YawPitch(glm::vec2(0.0f, 0.0f)), m_Speed(2.5f),
		m_CubeColor(glm::vec3(1.0f, 1.0f, 1.0f)), m_LampAmbient(glm::vec3(0.2f, 0.2f, 0.2f)), m_LampDiffuse(glm::vec3(0.5f, 0.5f, 0.5f)), m_LampSpecular(glm::vec3(1.0f, 1.0f, 1.0f)),
		m_SpecularPower(5.0f)
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

		//Face Culling
		GLCall(glEnable(GL_CULL_FACE));

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
		m_Shader = std::make_unique<Shader>("res/shaders/Phong.shader");
		m_Shader->Bind();
		m_Shader->SetUniform1i("u_Material.diffuse", 0);
		m_Shader->SetUniform1i("u_Material.specular", 1);
		m_Shader->UnBind();

		m_DepthShader = std::make_unique<Shader>("res/shaders/Depth.shader");

		m_StencilShader = std::make_unique<Shader>("res/shaders/Stencil.shader");

		m_LampShader = std::make_unique<Shader>("res/shaders/Lamp.shader");

		//Load Texture
		m_TextureDiffuse = std::make_unique<Texture>("res/textures/Box_diffuse.png");
		m_TextureSpecular = std::make_unique<Texture>("res/textures/Box_specular.png");

		

		//Set Camera
		Camera::GetInstance().Reset(glm::vec3(0.0f, 0.0f, 3.0f), glm::vec3(0.0f, 0.0f, -3.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	}

	TestDepthStencilBlending::~TestDepthStencilBlending()
	{
	}

	void TestDepthStencilBlending::OnUpdate(float deltaTime)
	{
		Camera::GetInstance().UpdateSpeed(m_Speed);
	}

	void TestDepthStencilBlending::OnRender()
	{
		GLCall(glClearColor(0.0f, 0.0f, 0.0f, 1.0f));
		GLCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT));

		Renderer renderer;

		//process user input

		//Move Camera
		Camera::GetInstance().SetYawPitch(m_YawPitch.x - 90.0f, m_YawPitch.y);
		m_Proj = glm::perspective(glm::radians(m_FOV), 960.0f / 540.0f, 0.1f, 100.0f);

		if (!(m_DepthCheckbox || m_StencilCheckbox)) {
			// Load the box
			m_Shader->Bind();

			m_TextureDiffuse->Bind();
			m_TextureSpecular->Bind(1);

			m_Shader->SetUniform3f("u_ObjectColor", m_CubeColor.x, m_CubeColor.y, m_CubeColor.z);

			m_Shader->SetUniform3f("u_ViewPos", Camera::GetInstance().CameraPosition().x, Camera::GetInstance().CameraPosition().y, Camera::GetInstance().CameraPosition().z);

			m_Shader->SetUniform1i("u_NumPointLights", 1);

			m_Shader->SetUniform3f("u_PointLight[0].position", m_LampPos.x, m_LampPos.y, m_LampPos.z);
			m_Shader->SetUniform3f("u_PointLight[0].ambient", m_LampAmbient.x, m_LampAmbient.y, m_LampAmbient.z);
			m_Shader->SetUniform3f("u_PointLight[0].diffuse", m_LampDiffuse.x, m_LampDiffuse.y, m_LampDiffuse.z);
			m_Shader->SetUniform3f("u_PointLight[0].specular", m_LampSpecular.x, m_LampSpecular.y, m_LampSpecular.z);
			m_Shader->SetUniform1f("u_PointLight[0].constant", 1.0f);
			m_Shader->SetUniform1f("u_PointLight[0].linear", 0.09f);
			m_Shader->SetUniform1f("u_PointLight[0].quadratic", 0.032f);

			m_Shader->SetUniform1f("u_Material.shininess", m_SpecularPower);

			for (unsigned int i = 0; i < 10; i++)
			{
				//create model matrix
				glm::mat4 model = glm::mat4(1.0f);

				model = glm::translate(model, m_CubePos + cubePositions[i]);
				model = glm::rotate(model, (float)glfwGetTime() * glm::radians(20.0f + i * 5.0f), glm::vec3(0.5f, 1.0f, 0.0f));

				m_View = Camera::GetInstance().viewMatrix;

				//construt model view projection
				glm::mat4 mvp = m_Proj * m_View * model;

				m_Shader->SetUniformMat4f("u_MVP", mvp);
				m_Shader->SetUniformMat4f("u_Model", model);

				//draw texture
				renderer.Draw(*m_VAO, *m_IndexBuffer, *m_Shader);
			}

			m_TextureDiffuse->UnBind();
			m_Shader->UnBind();

			// Load the Lamp
			m_LampShader->Bind();

			//Move Camera
			Camera::GetInstance().SetYawPitch(m_YawPitch.x - 90.0f, m_YawPitch.y);
			m_Proj = glm::perspective(glm::radians(m_FOV), 960.0f / 540.0f, 0.1f, 100.0f);

			//create model matrix
			glm::mat4 model = glm::mat4(1.0f);

			model = glm::translate(model, m_CubePos + m_LampPos);
			model = glm::scale(model, glm::vec3(0.2f));

			m_View = Camera::GetInstance().viewMatrix;

			//construt model view projection
			glm::mat4 mvp = m_Proj * m_View * model;

			m_LampShader->SetUniformMat4f("u_MVP", mvp);

			//draw texture
			renderer.Draw(*m_VAO, *m_IndexBuffer, *m_LampShader);

			m_LampShader->UnBind();
		}
		else if (m_DepthCheckbox)
		{
			m_DepthShader->Bind();

			for (unsigned int i = 0; i < 10; i++)
			{
				//create model matrix
				glm::mat4 model = glm::mat4(1.0f);

				model = glm::translate(model, m_CubePos + cubePositions[i]);
				model = glm::rotate(model, (float)glfwGetTime() * glm::radians(20.0f + i * 5.0f), glm::vec3(0.5f, 1.0f, 0.0f));

				m_View = Camera::GetInstance().viewMatrix;

				//construt model view projection
				glm::mat4 mvp = m_Proj * m_View * model;

				m_Shader->SetUniformMat4f("u_MVP", mvp);

				//draw texture
				renderer.Draw(*m_VAO, *m_IndexBuffer, *m_DepthShader);
			}

			m_DepthShader->UnBind();
		}
		else
		{
			// Load the Lamp
			glStencilMask(0x00);

			m_LampShader->Bind();

			//Move Camera
			Camera::GetInstance().SetYawPitch(m_YawPitch.x - 90.0f, m_YawPitch.y);
			m_Proj = glm::perspective(glm::radians(m_FOV), 960.0f / 540.0f, 0.1f, 100.0f);

			//create model matrix
			glm::mat4 model = glm::mat4(1.0f);

			model = glm::translate(model, m_LampPos);
			model = glm::scale(model, glm::vec3(0.2f));

			m_View = Camera::GetInstance().viewMatrix;

			//construt model view projection
			glm::mat4 mvp = m_Proj * m_View * model;

			m_LampShader->SetUniformMat4f("u_MVP", mvp);

			//draw texture
			renderer.Draw(*m_VAO, *m_IndexBuffer, *m_LampShader);

			m_LampShader->UnBind();

			//Load Box
			glStencilFunc(GL_ALWAYS, 1, 0xFF);
			glStencilMask(0xFF);

			m_Shader->Bind();

			m_TextureDiffuse->Bind();
			m_TextureSpecular->Bind(1);

			m_Shader->SetUniform3f("u_ObjectColor", m_CubeColor.x, m_CubeColor.y, m_CubeColor.z);

			m_Shader->SetUniform3f("u_ViewPos", Camera::GetInstance().CameraPosition().x, Camera::GetInstance().CameraPosition().y, Camera::GetInstance().CameraPosition().z);

			m_Shader->SetUniform1i("u_NumPointLights", 1);

			m_Shader->SetUniform3f("u_PointLight[0].position", m_LampPos.x, m_LampPos.y, m_LampPos.z);
			m_Shader->SetUniform3f("u_PointLight[0].ambient", m_LampAmbient.x, m_LampAmbient.y, m_LampAmbient.z);
			m_Shader->SetUniform3f("u_PointLight[0].diffuse", m_LampDiffuse.x, m_LampDiffuse.y, m_LampDiffuse.z);
			m_Shader->SetUniform3f("u_PointLight[0].specular", m_LampSpecular.x, m_LampSpecular.y, m_LampSpecular.z);
			m_Shader->SetUniform1f("u_PointLight[0].constant", 1.0f);
			m_Shader->SetUniform1f("u_PointLight[0].linear", 0.09f);
			m_Shader->SetUniform1f("u_PointLight[0].quadratic", 0.032f);

			m_Shader->SetUniform1f("u_Material.shininess", m_SpecularPower);

			for (unsigned int i = 0; i < 10; i++)
			{
				//create model matrix
				glm::mat4 model = glm::mat4(1.0f);

				model = glm::translate(model, m_CubePos + cubePositions[i]);
				model = glm::rotate(model, (float)glfwGetTime() * glm::radians(20.0f + i * 5.0f), glm::vec3(0.5f, 1.0f, 0.0f));

				m_View = Camera::GetInstance().viewMatrix;

				//construt model view projection
				glm::mat4 mvp = m_Proj * m_View * model;

				m_Shader->SetUniformMat4f("u_MVP", mvp);
				m_Shader->SetUniformMat4f("u_Model", model);

				//draw texture
				renderer.Draw(*m_VAO, *m_IndexBuffer, *m_Shader);
			}

			m_TextureDiffuse->UnBind();
			m_Shader->UnBind();

			glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
			glStencilMask(0x00);
			glDisable(GL_DEPTH_TEST);

			m_StencilShader->Bind();

			for (unsigned int i = 0; i < 10; i++)
			{
				//create model matrix
				glm::mat4 model = glm::mat4(1.0f);

				model = glm::translate(model, m_CubePos + cubePositions[i]);
				model = glm::scale(model, glm::vec3(1.1f));
				model = glm::rotate(model, (float)glfwGetTime() * glm::radians(20.0f + i * 5.0f), glm::vec3(0.5f, 1.0f, 0.0f));

				m_View = Camera::GetInstance().viewMatrix;

				//construt model view projection
				glm::mat4 mvp = m_Proj * m_View * model;

				m_Shader->SetUniformMat4f("u_MVP", mvp);

				//draw texture
				renderer.Draw(*m_VAO, *m_IndexBuffer, *m_StencilShader);
			}
				
			m_StencilShader->UnBind();

			glStencilFunc(GL_ALWAYS, 1, 0xFF);
			glStencilMask(0xFF);
			glEnable(GL_DEPTH_TEST);
		}
	}

	void TestDepthStencilBlending::MouseCallback(GLFWwindow* window, double xpos, double ypos)
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

		Camera::GetInstance().UpdateYawPitch((float) xOffset,(float) yOffset);
	}

	void TestDepthStencilBlending::ScrollCallback(GLFWwindow* window, double xoffset, double yoffset)
	{
		if (m_FOV >= 1.0f && m_FOV <= 45.0f)
			m_FOV -= yoffset;
		if (m_FOV <= 1.0f)
			m_FOV = 1.0f;
		if (m_FOV >= 45.0f)
			m_FOV = 45.0f;

		m_Proj = glm::perspective(glm::radians(m_FOV), 960.0f / 540.0f, 0.1f, 100.0f);
	}

	void TestDepthStencilBlending::OnImGuiRender()
	{
		ImGui::TextWrapped("Welcome to the Depth, Stencil, and Blending Test Enviroment.");

		ImGui::Checkbox("Depth Shader (darker = closer)", &m_DepthCheckbox);
		ImGui::Checkbox("Stencil Shader", &m_StencilCheckbox);

		if (ImGui::CollapsingHeader("Camera Options")) {
			ImGui::SliderFloat2("Yaw Pitch", &m_YawPitch.x, -89.0f, 89.0f);
			ImGui::SliderFloat("FOV", &m_FOV, 1.0f, 89.0f);
			ImGui::SliderFloat("Speed", &m_Speed, 0.5f, 5.0f);
		}

		if (ImGui::CollapsingHeader("Cube Options")) {
			ImGui::SliderFloat3("Translate Cube", &m_CubePos.x, -5.0f, 5.0f);
			ImGui::ColorEdit3("Cube Color", &m_CubeColor.x);
			ImGui::SliderFloat("Specular Power", &m_SpecularPower, 0.0f, 8.0f);
		}

		if (ImGui::CollapsingHeader("Lamp Options")) {
			ImGui::SliderFloat3("Translate Lamp", &m_LampPos.x, -5.0f, 5.0f);
			ImGui::ColorEdit3("Light Ambient", &m_LampAmbient.x);
			ImGui::ColorEdit3("Light Diffuse", &m_LampDiffuse.x);
			ImGui::ColorEdit3("Light Specular", &m_LampSpecular.x);
		}

		//displays framerate
		ImGui::TextWrapped("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
	}
}