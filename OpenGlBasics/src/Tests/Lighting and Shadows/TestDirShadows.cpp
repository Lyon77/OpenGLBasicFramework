#include "TestDirShadows.h"

#include <iostream>

#include "Renderer.h"
#include "imgui/imgui.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

namespace test {

	TestDirShadows::TestDirShadows()
		: m_Proj(glm::perspective(glm::radians(45.0f), 960.0f / 540.0f, 0.1f, 100.0f)), m_View(glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, -3.0))), //glm::perspective(glm::radians(45.0f), 960.0f / 540.0f, 0.1f, 500.0f)
		m_CubePos(0, 0, 0), m_FOV(45.0f), m_YawPitch(glm::vec2(0.0f, 0.0f)), m_Speed(2.5f),
		m_CubeColor(glm::vec3(1.0f, 1.0f, 1.0f)),
		m_LampAmbient(glm::vec3(0.2f, 0.2f, 0.2f)), m_LampDiffuse(glm::vec3(0.5f, 0.5f, 0.5f)), m_LampSpecular(glm::vec3(1.0f, 1.0f, 1.0f)),
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
		m_VertexBuffer = std::make_unique<VertexBuffer>(positions, 8 * 4 * 6 * sizeof(float)); //8 values, 4 points, 6 faces
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
		m_IndexBuffer = std::make_unique<IndexBuffer>(indicies, 6 * 6);
		m_FrameIndexBuffer = std::make_unique<IndexBuffer>(frameScreenIndicies, 6 * 1);

		//create Vertex and Fragment source
		m_Shader = std::make_unique<Shader>("res/shaders/Shadow.shader");

		m_LampShader = std::make_unique<Shader>("res/shaders/Lamp.shader");

		m_DirShader = std::make_unique<Shader>("res/shaders/ShadowDir.shader");

		m_Shader->Bind();

		//Load Texture
		m_TextureDiffuse = std::make_unique<Texture>("res/textures/Box_diffuse.png");
		m_TextureSpecular = std::make_unique<Texture>("res/textures/Box_specular.png");

		m_Shader->SetUniform1i("u_HasDirLight", true);
		m_Shader->SetUniform1i("u_Gamma", false);
		m_Shader->SetUniform1i("u_Material.diffuse", 0);
		m_Shader->SetUniform1i("u_Material.specular", 1);
		m_Shader->SetUniform1i("u_ShadowMap", 2);
		m_Shader->SetUniform1i("u_ShadowCubeMap", 3);

		//Set up FrameBuffer
		m_FrameBuffer = std::make_unique<FrameBuffer>();
		m_FrameBuffer->AddDepthAttachment();

		//Set Camera
		Camera::GetInstance().Reset(glm::vec3(0.0f, 0.0f, 6.0f), glm::vec3(0.0f, 0.0f, -3.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	}

	TestDirShadows::~TestDirShadows()
	{
	}

	void TestDirShadows::OnUpdate(float deltaTime)
	{
		Camera::GetInstance().UpdateSpeed(m_Speed);
	}

	void TestDirShadows::OnRender()
	{
		Renderer renderer;

		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		GLCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
		GLCall(glViewport(0, 0, 1024, 1024));

		//Configure Matricies and Shaders
		float near_plane = 0.1f, far_plane = 100.0f;
		glm::mat4 lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, near_plane, far_plane);

		glm::mat4 lightView = glm::lookAt(m_LampPos, glm::vec3(0.0f), glm::vec3(0.0, 1.0, 0.0));
		glm::mat4 lightSpaceMatrix = lightProjection * lightView;

		//Set FrameBuffer to the created one
		m_FrameBuffer->Bind();
		glClear(GL_DEPTH_BUFFER_BIT);

		m_DirShader->Bind();

		//Render Scene using Shadow Depth Shader
		for (unsigned int i = 0; i < 10; i++)
		{
			//create model matrix
			glm::mat4 model = glm::mat4(1.0f);
			model = glm::translate(model, m_CubePos + cubePositions[i]);

			////construt model view projection
			glm::mat4 mvp = lightSpaceMatrix * model;
			m_DirShader->SetUniformMat4f("u_MVP", mvp);

			//draw texture
			renderer.Draw(*m_VAO, *m_IndexBuffer, *m_DirShader);
		}

		m_DirShader->UnBind();
		m_FrameBuffer->UnBind();


		glViewport(0, 0, 960, 540);
		GLCall(glClearColor(0.0f, 0.0f, 0.0f, 1.0f));
		GLCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT));

		//process user input

		// Load the box	
		{
			m_Shader->Bind();

			m_TextureDiffuse->Bind();
			m_TextureSpecular->Bind(1);
			m_FrameBuffer->BindDepthTexture(2);

			m_Shader->SetUniformMat4f("u_LightSpaceMatrix", lightSpaceMatrix);

			m_Shader->SetUniform3f("u_ObjectColor", m_CubeColor.x, m_CubeColor.y, m_CubeColor.z);

			m_Shader->SetUniform3f("u_ViewPos", Camera::GetInstance().CameraPosition().x, Camera::GetInstance().CameraPosition().y, Camera::GetInstance().CameraPosition().z);

			m_Shader->SetUniform3f("u_DirLight.direction", -m_LampPos.x, -m_LampPos.y, -m_LampPos.z);
			m_Shader->SetUniform3f("u_DirLight.ambient", m_LampAmbient.x, m_LampAmbient.y, m_LampAmbient.z);
			m_Shader->SetUniform3f("u_DirLight.diffuse", m_LampDiffuse.x, m_LampDiffuse.y, m_LampDiffuse.z);
			m_Shader->SetUniform3f("u_DirLight.specular", m_LampSpecular.x, m_LampSpecular.y, m_LampSpecular.z);

			m_Shader->SetUniform1f("u_Material.shininess", m_SpecularPower);

			//Move Camera
			Camera::GetInstance().SetYawPitch(m_YawPitch.x - 90.0f, m_YawPitch.y);
			m_Proj = glm::perspective(glm::radians(m_FOV), 960.0f / 540.0f, 0.1f, 100.0f);

			for (unsigned int i = 0; i < 10; i++)
			{
				//create model matrix
				glm::mat4 model = glm::mat4(1.0f);

				model = glm::translate(model, m_CubePos + cubePositions[i]);
				m_View = Camera::GetInstance().viewMatrix;

				//construt model view projection
				glm::mat4 mvp = m_Proj * m_View * model;

				m_Shader->SetUniformMat4f("u_MVP", mvp);
				m_Shader->SetUniformMat4f("u_Model", model);

				//draw texture
				renderer.Draw(*m_VAO, *m_IndexBuffer, *m_Shader);
			}

			m_FrameBuffer->UnBindCubeTexture();
			m_TextureDiffuse->UnBind();
			m_Shader->UnBind();
		}
	}

	void TestDirShadows::MouseCallback(GLFWwindow* window, double xpos, double ypos)
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

	void TestDirShadows::ScrollCallback(GLFWwindow* window, double xoffset, double yoffset)
	{
		if (m_FOV >= 1.0f && m_FOV <= 45.0f)
			m_FOV -= yoffset;
		if (m_FOV <= 1.0f)
			m_FOV = 1.0f;
		if (m_FOV >= 45.0f)
			m_FOV = 45.0f;

		m_Proj = glm::perspective(glm::radians(m_FOV), 960.0f / 540.0f, 0.1f, 100.0f);
	}

	void TestDirShadows::OnImGuiRender()
	{
		ImGui::TextWrapped("Welcome to the Shadow Test Enviroment. Use WASD to move around and QE to zoom in and out so that you can see the shadows. There are more setting options below.");
		if (ImGui::CollapsingHeader("Cube Options")) {
			ImGui::SliderFloat3("Translate Cube", &m_CubePos.x, -5.0f, 5.0f);
			ImGui::ColorEdit3("Cube Color", &m_CubeColor.x);
			ImGui::SliderFloat("Specular Power", &m_SpecularPower, 0.0f, 8.0f);
		}

		if (ImGui::CollapsingHeader("Lamp Options")) {
			ImGui::SliderFloat3("Light Position", &m_LampPos.x, -1.0f, 1.0f);
			ImGui::ColorEdit3("Light Ambient", &m_LampAmbient.x);
			ImGui::ColorEdit3("Light Diffuse", &m_LampDiffuse.x);
			ImGui::ColorEdit3("Light Specular", &m_LampSpecular.x);
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