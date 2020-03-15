#include "TestDeferredShading.h"

#include <iostream>

#include "Renderer.h"
#include "Window.h"
#include "imgui/imgui.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

namespace test {

	TestDeferredShading::TestDeferredShading()
		: m_Proj(glm::perspective(glm::radians(45.0f), 960.0f / 540.0f, 0.1f, 100.0f)), m_View(glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, -3.0))), //glm::perspective(glm::radians(45.0f), 960.0f / 540.0f, 0.1f, 500.0f)
		m_FOV(45.0f), m_YawPitch(glm::vec2(0.0f, 0.0f)), m_Speed(2.5f),
		m_LampAmbient(glm::vec3(0.2f, 0.2f, 0.2f)), m_LampDiffuse(glm::vec3(0.5f, 0.5f, 0.5f)), m_LampSpecular(glm::vec3(1.0f, 1.0f, 1.0f)),
		m_SpecularPower(5.0f),
		m_AttenuationCheckbox(true)
	{
		//Depth Testing
		GLCall(glEnable(GL_DEPTH_TEST));

		//Blending for transparent bits in the texture
		GLCall(glDisable(GL_BLEND));

		m_ObjectPositions.push_back(glm::vec3(-3.0f, -0.0f, -3.0f));
		m_ObjectPositions.push_back(glm::vec3( 0.0f, -0.0f, -3.0f));
		m_ObjectPositions.push_back(glm::vec3( 3.0f, -0.0f, -3.0f));
		m_ObjectPositions.push_back(glm::vec3(-3.0f, -0.0f,  0.0f));
		m_ObjectPositions.push_back(glm::vec3( 0.0f, -0.0f,  0.0f));
		m_ObjectPositions.push_back(glm::vec3( 3.0f, -0.0f,  0.0f));
		m_ObjectPositions.push_back(glm::vec3(-3.0f, -0.0f,  3.0f));
		m_ObjectPositions.push_back(glm::vec3( 0.0f, -0.0f,  3.0f));
		m_ObjectPositions.push_back(glm::vec3( 3.0f, -0.0f,  3.0f));

		const unsigned int NR_LIGHTS = 32;
		srand(13);
		for (unsigned int i = 0; i < NR_LIGHTS; i++)
		{
			// calculate slightly random offsets
			float xPos = ((rand() % 100) / 100.0) * 6.0 - 3.0;
			float yPos = ((rand() % 100) / 100.0) * 6.0 - 4.0;
			float zPos = ((rand() % 100) / 100.0) * 6.0 - 3.0;
			m_LightPositions.push_back(glm::vec3(xPos, yPos, zPos));
			// also calculate random color
			float rColor = ((rand() % 100) / 200.0f) + 0.5; // between 0.5 and 1.0
			float gColor = ((rand() % 100) / 200.0f) + 0.5; // between 0.5 and 1.0
			float bColor = ((rand() % 100) / 200.0f) + 0.5; // between 0.5 and 1.0
			m_LightColors.push_back(glm::vec3(rColor, gColor, bColor));
		}

		// Cube generation ------------------------------------------------------
		float cubePositions[] = {
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

		unsigned int cubeIndices[] = {
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

		//create vertex array
		m_CubeVAO = std::make_unique<VertexArray>();

		//create vertex buffer
		m_CubeVertexBuffer = std::make_unique<VertexBuffer>(cubePositions, 8 * 4 * 6 * sizeof(float)); //3 values, 4, points, 6 faces

		//set vertex buffer to array
		VertexBufferLayout cubeLayout;
		cubeLayout.Push<float>(3);
		cubeLayout.Push<float>(3);
		cubeLayout.Push<float>(2);
		m_CubeVAO->AddBuffer(*m_CubeVertexBuffer, cubeLayout);

		//create index buffer
		m_CubeIndexBuffer = std::make_unique<IndexBuffer>(cubeIndices, 6 * 6);

		// Quad generation ----------------------------------------------------------------
		float quadPositions[] = {
			 -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
			  1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
			  1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
			 -1.0f,  1.0f, 0.0f, 0.0f, 1.0f
		};

		unsigned int quadIndices[] = {
			0, 1, 2,
			2, 3, 0
		};

		m_QuadVAO = std::make_unique<VertexArray>();

		m_QuadVertexBuffer = std::make_unique<VertexBuffer>(quadPositions, 5 * 4 * sizeof(float)); //4 values, 4, points

		VertexBufferLayout quadLayout;
		quadLayout.Push<float>(3);
		quadLayout.Push<float>(2);
		m_QuadVAO->AddBuffer(*m_QuadVertexBuffer, quadLayout);

		m_QuadIndexBuffer = std::make_unique<IndexBuffer>(quadIndices, 6);

		//create Vertex and Fragment source ------------------------------------------------
		m_GBufferShader = std::make_unique<Shader>("res/shaders/GBuffer.shader");
		m_QuadShader = std::make_unique<Shader>("res/shaders/Basic.shader");
		m_LightingShader = std::make_unique<Shader>("res/shaders/DeferredLightingPass.shader");
		m_LampShader = std::make_unique<Shader>("res/shaders/Lamp.shader");

		m_GBufferShader->Bind();
		m_GBufferShader->SetUniform1i("texture_diffuse1", 0);
		m_GBufferShader->SetUniform1i("texture_specular1", 1);
		m_GBufferShader->UnBind();

		m_QuadShader->Bind();
		m_QuadShader->SetUniform1i("u_Texture", 0);
		m_QuadShader->UnBind();

		m_LightingShader->Bind();
		m_LightingShader->SetUniform1i("u_Position", 0);
		m_LightingShader->SetUniform1i("u_Normal", 1);
		m_LightingShader->SetUniform1i("u_AlbedoSpec", 2);
		m_LightingShader->UnBind();

		// Set FrameBuffer
		m_FBO = std::make_unique<FrameBuffer>();
		m_FBO->AddColorAttachment(0, GL_RGB16F, GL_RGB, GL_FLOAT); // Position
		m_FBO->AddColorAttachment(1, GL_RGB16F, GL_RGB, GL_FLOAT); // Normal
		m_FBO->AddColorAttachment(2, GL_RGBA, GL_RGBA, GL_UNSIGNED_BYTE); // Color + specular color
		m_FBO->AddDepthAttachment();
		m_FBO->AddRenderBufferAttachment(3);

		m_Model = std::make_unique<Model>("res/models/nanosuit/nanosuit.obj");

		//Set Camera
		Camera::GetInstance().Reset(glm::vec3(0.0f, 10.0f, 15.0f), glm::vec3(0.0f, 5.0f, -3.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	}

	TestDeferredShading::~TestDeferredShading()
	{
	}

	void TestDeferredShading::OnUpdate(float deltaTime)
	{
		Camera::GetInstance().UpdateSpeed(m_Speed);
	}

	void TestDeferredShading::OnRender()
	{
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		Renderer renderer;

		//Move Camera
		Camera::GetInstance().SetYawPitch(m_YawPitch.x - 90.0f, m_YawPitch.y);
		m_Proj = glm::perspective(glm::radians(m_FOV), 960.0f / 540.0f, 0.1f, 100.0f);

		// Perform G-Buffer ----------------------------------------------------

		m_FBO->Bind();
		GLCall(glClearColor(0.0f, 0.0f, 0.0f, 1.0f));
		GLCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT));

		{
			m_GBufferShader->Bind();

			for (unsigned int i = 0; i < m_ObjectPositions.size(); i++)
			{
				//create model matrix
				glm::mat4 model = glm::mat4(1.0f);
				model = glm::translate(model, m_ObjectPositions[i]);
				
				m_View = Camera::GetInstance().viewMatrix;
				glm::mat4 mvp = m_Proj * m_View * model;

				m_GBufferShader->SetUniformMat4f("u_MVP", mvp);
				m_GBufferShader->SetUniformMat4f("u_Model", model);

				//draw texture
				m_Model->Draw(m_GBufferShader.get());
			}

			m_GBufferShader->UnBind();
		}
		m_FBO->UnBind();

		// Light Pass --------------------------------------------------------------

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		{
			m_LightingShader->Bind();
			m_FBO->BindColorTexture(0, 0);
			m_FBO->BindColorTexture(1, 1);
			m_FBO->BindColorTexture(2, 2);

			m_LightingShader->SetUniform3f("u_ViewPos", Camera::GetInstance().CameraPosition().x, Camera::GetInstance().CameraPosition().y, Camera::GetInstance().CameraPosition().z);
			m_LightingShader->SetUniform1i("u_NbrLights", m_LightPositions.size());

			for (unsigned int i = 0; i < m_LightPositions.size(); i++)
			{
				m_LightingShader->SetUniform3f("u_Lights[" + std::to_string(i) + "].Position", (m_LightPositions[i].x + m_LampPos.x), (m_LightPositions[i].y + m_LampPos.y), (m_LightPositions[i].z + m_LampPos.z));
				m_LightingShader->SetUniform3f("u_Lights[" + std::to_string(i) + "].Color", m_LightColors[i].r, m_LightColors[i].g, m_LightColors[i].b);

				// update attenuation parameters and calculate radius
				const float constant = 1.0; // note that we don't send this to the shader, we assume it is always 1.0 (in our case)
				const float linear = 0.7;
				const float quadratic = 1.8;
				m_LightingShader->SetUniform1f("u_Lights[" + std::to_string(i) + "].Linear", linear);
				m_LightingShader->SetUniform1f("u_Lights[" + std::to_string(i) + "].Quadratic", quadratic);
				// then calculate radius of light volume/sphere
				const float maxBrightness = std::fmaxf(std::fmaxf(m_LightColors[i].r, m_LightColors[i].g), m_LightColors[i].b);
				float radius = (-linear + std::sqrt(linear * linear - 4 * quadratic * (constant - (256.0f / 5.0f) * maxBrightness))) / (2.0f * quadratic);
				m_LightingShader->SetUniform1f("u_Lights[" + std::to_string(i) + "].Radius", radius);
			}

			renderer.Draw(*m_QuadVAO, *m_QuadIndexBuffer, *m_LightingShader);

			m_LightingShader->UnBind();
		}
		

		// copy content of geometry's depth buffer to default framebuffer's depth buffer -------------------
		m_FBO->Bind();
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0); // write to default framebuffer
		glBlitFramebuffer(0, 0, Window::GetInstance().GetWidth(), Window::GetInstance().GetHeight(), 0, 0, Window::GetInstance().GetWidth(), Window::GetInstance().GetHeight(), GL_DEPTH_BUFFER_BIT, GL_NEAREST);
		m_FBO->UnBind();

		// Render Lights on top of the scene -------------------------------
		{
			m_LampShader->Bind();

			for (unsigned int i = 0; i < m_LightPositions.size(); i++)
			{
				//create model matrix
				glm::mat4 model = glm::mat4(1.0f);
				model = glm::translate(model, m_LampPos + m_LightPositions[i]);
				model = glm::scale(model, glm::vec3(0.25f));
				m_View = Camera::GetInstance().viewMatrix;
				glm::mat4 mvp = m_Proj * m_View * model;

				m_LampShader->SetUniformMat4f("u_MVP", mvp);
				m_LampShader->SetUniform4f("u_Color", m_LightColors[i].r, m_LightColors[i].g, m_LightColors[i].b, 1.0f);

				renderer.Draw(*m_CubeVAO, *m_CubeIndexBuffer, *m_LampShader);
			}


			m_LampShader->UnBind();
		}
	}

	void TestDeferredShading::MouseCallback(GLFWwindow* window, double xpos, double ypos)
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

		Camera::GetInstance().UpdateYawPitch(xOffset, yOffset);
	}

	void TestDeferredShading::ScrollCallback(GLFWwindow* window, double xoffset, double yoffset)
	{
		if (m_FOV >= 1.0f && m_FOV <= 45.0f)
			m_FOV -= yoffset;
		if (m_FOV <= 1.0f)
			m_FOV = 1.0f;
		if (m_FOV >= 45.0f)
			m_FOV = 45.0f;

		m_Proj = glm::perspective(glm::radians(m_FOV), 960.0f / 540.0f, 0.1f, 100.0f);
	}

	void TestDeferredShading::OnImGuiRender()
	{
		ImGui::TextWrapped("Welcome to the Deferred Lighting Test Enviroment. This uses Blinn-Phong lighting. Use WASD to move around and QE to zoom in and out. There are more setting options below.");

		if (ImGui::CollapsingHeader("Lamp Options")) {
			ImGui::SliderFloat3("Translate Lamp", &m_LampPos.x, -5.0f, 20.0f);
			ImGui::ColorEdit3("Light Ambient", &m_LampAmbient.x);
			ImGui::ColorEdit3("Light Diffuse", &m_LampDiffuse.x);
			ImGui::ColorEdit3("Light Specular", &m_LampSpecular.x);
			ImGui::Checkbox("Attenuation", &m_AttenuationCheckbox);
		}

		if (ImGui::CollapsingHeader("Camera Options")) {
			ImGui::SliderFloat2("Yaw Pitch", &m_YawPitch.x, -89.0f, 89.0f);
			ImGui::SliderFloat("FOV", &m_FOV, 1.0f, 89.0f);
			ImGui::SliderFloat("Speed", &m_Speed, 0.5f, 5.0f);
		}

		//displays framerate
		ImGui::TextWrapped("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
	}
}