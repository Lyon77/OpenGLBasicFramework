#include "TestNormalMaps.h"

#include <iostream>

#include "Renderer.h"
#include "imgui/imgui.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

namespace test {

	TestNormalMaps::TestNormalMaps()
		: m_Proj(glm::perspective(glm::radians(45.0f), 960.0f / 540.0f, 0.1f, 100.0f)), m_View(glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, -3.0))), //glm::perspective(glm::radians(45.0f), 960.0f / 540.0f, 0.1f, 500.0f)
		m_CubeRotation(glm::vec3(0.0, 0.0, 0.0)), m_FOV(45.0f), m_YawPitch(glm::vec2(0.0f, 0.0f)), m_Speed(2.5f),
		m_LampAmbient(glm::vec3(0.2f, 0.2f, 0.2f)), m_LampDiffuse(glm::vec3(0.5f, 0.5f, 0.5f)), m_LampSpecular(glm::vec3(1.0f, 1.0f, 1.0f)),
		m_SpecularPower(5.0f)
	{

		// positions
		glm::vec3 pos1(-1.0, 1.0, 0.0);
		glm::vec3 pos2(-1.0, -1.0, 0.0);
		glm::vec3 pos3(1.0, -1.0, 0.0);
		glm::vec3 pos4(1.0, 1.0, 0.0);
		// texture coordinates
		glm::vec2 uv1(0.0, 1.0);
		glm::vec2 uv2(0.0, 0.0);
		glm::vec2 uv3(1.0, 0.0);
		glm::vec2 uv4(1.0, 1.0);
		// normal vector
		glm::vec3 nm(0.0, 0.0, 1.0);

		// calculate tangent/bitangent vectors of both triangles
		glm::vec3 tangent1, bitangent1;
		glm::vec3 tangent2, bitangent2;

		// triangle 
		glm::vec3 edge1 = pos2 - pos1;
		glm::vec3 edge2 = pos3 - pos1;
		glm::vec2 deltaUV1 = uv2 - uv1;
		glm::vec2 deltaUV2 = uv3 - uv1;

		float f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);

		tangent1.x = f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
		tangent1.y = f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
		tangent1.z = f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);
		tangent1 = glm::normalize(tangent1);

		// triangle 2
		edge1 = pos3 - pos1;
		edge2 = pos4 - pos1;
		deltaUV1 = uv3 - uv1;
		deltaUV2 = uv4 - uv1;

		f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);

		tangent2.x = f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
		tangent2.y = f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
		tangent2.z = f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);
		tangent2 = glm::normalize(tangent2);

		//Define Triangle
		// FIX ME: try to get the positions to be from 960 by 540
		float positions[] = {
			//position              //normal          //texture
			pos1.x, pos1.y, pos1.z, nm.x, nm.y, nm.z, uv1.x, uv1.y, tangent1.x, tangent1.y, tangent1.z,
			pos2.x, pos2.y, pos2.z, nm.x, nm.y, nm.z, uv2.x, uv2.y, tangent1.x, tangent1.y, tangent1.z,
			pos3.x, pos3.y, pos3.z, nm.x, nm.y, nm.z, uv3.x, uv3.y, tangent1.x, tangent1.y, tangent1.z,
			pos4.x, pos4.y, pos4.z, nm.x, nm.y, nm.z, uv4.x, uv4.y, tangent2.x, tangent2.y, tangent2.z
		};

		unsigned int indicies[] = {
			0, 2, 1,
			2, 0, 3
		};

		//Depth Testing
		GLCall(glEnable(GL_DEPTH_TEST));

		//Blending for transparent bits in the texture
		GLCall(glEnable(GL_BLEND));
		GLCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));

		m_VAO = std::make_unique<VertexArray>();

		m_VertexBuffer = std::make_unique<VertexBuffer>(positions, 11 * 4 * 1 * sizeof(float)); //11 values, 4 points, 1 faces

		//set vertex buffer to array
		VertexBufferLayout layout;
		layout.Push<float>(3);
		layout.Push<float>(3);
		layout.Push<float>(2);
		layout.Push<float>(3);
		m_VAO->AddBuffer(*m_VertexBuffer, layout);

		m_IndexBuffer = std::make_unique<IndexBuffer>(indicies, 6 * 1);

		//create Vertex and Fragment source
		m_Shader = std::make_unique<Shader>("res/shaders/NormalMap.shader");
		m_Shader->Bind();
		m_Shader->SetUniform1i("u_Texture", 0);
		m_Shader->SetUniform1i("u_NormalMap", 1);
		m_Shader->UnBind();

		m_LampShader = std::make_unique<Shader>("res/shaders/Lamp.shader");

		m_TextureDiffuse = std::make_unique<Texture>("res/textures/Brickwall.jpg");
		m_NormalMap = std::make_unique<Texture>("res/textures/Brickwall_normal.jpg");



		//Set Camera
		m_Camera = std::make_unique<Camera>(glm::vec3(0.0f, 0.0f, 3.0f), glm::vec3(0.0f, 0.0f, -3.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	}

	TestNormalMaps::~TestNormalMaps()
	{
	}

	void TestNormalMaps::OnUpdate(float deltaTime)
	{
	}

	void TestNormalMaps::OnRender()
	{
		Renderer renderer;
		GLCall(glClearColor(0.0f, 0.0f, 0.0f, 1.0f));
		GLCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT));

		//Move Camera
		m_Camera->SetYawPitch(m_YawPitch.x - 90.0f, m_YawPitch.y);
		m_Proj = glm::perspective(glm::radians(m_FOV), 960.0f / 540.0f, 0.1f, 100.0f);

		{
			m_Shader->Bind();
			m_TextureDiffuse->Bind();
			m_NormalMap->Bind(1);

			//Set Uniforms
			m_Shader->SetUniform3f("u_ViewPos", m_Camera->CameraPosition().x, m_Camera->CameraPosition().y, m_Camera->CameraPosition().z);
			m_Shader->SetUniform3f("u_PointLight.position", m_LampPos.x, m_LampPos.y, m_LampPos.z);
			m_Shader->SetUniform3f("u_PointLight.ambient", m_LampAmbient.x, m_LampAmbient.y, m_LampAmbient.z);
			m_Shader->SetUniform3f("u_PointLight.diffuse", m_LampDiffuse.x, m_LampDiffuse.y, m_LampDiffuse.z);
			m_Shader->SetUniform3f("u_PointLight.specular", m_LampSpecular.x, m_LampSpecular.y, m_LampSpecular.z);

			glm::mat4 model = glm::mat4(1.0f);
			model = glm::rotate(model, glm::radians(m_CubeRotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
			model = glm::rotate(model, glm::radians(m_CubeRotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
			model = glm::rotate(model, glm::radians(m_CubeRotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
			m_View = m_Camera->viewMatrix;

			//construt model view projection
			glm::mat4 mvp = m_Proj * m_View * model;

			m_Shader->SetUniformMat4f("u_MVP", mvp);
			m_Shader->SetUniformMat4f("u_Model", model);

			renderer.Draw(*m_VAO, *m_IndexBuffer, *m_Shader);

			m_TextureDiffuse->UnBind();
			m_Shader->UnBind();
		}

		// Load the Lamp
		{
			m_LampShader->Bind();

			//create model matrix
			glm::mat4 model = glm::mat4(1.0f);

			model = glm::translate(model, m_LampPos);
			model = glm::scale(model, glm::vec3(0.1f));

			m_View = m_Camera->viewMatrix;

			//construt model view projection
			glm::mat4 mvp = m_Proj * m_View * model;

			m_LampShader->SetUniformMat4f("u_MVP", mvp);

			//draw texture
			renderer.Draw(*m_VAO, *m_IndexBuffer, *m_LampShader);

			m_LampShader->UnBind();
		}
	}

	void TestNormalMaps::MouseCallback(GLFWwindow* window, double xpos, double ypos)
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

	void TestNormalMaps::ScrollCallback(GLFWwindow* window, double xoffset, double yoffset)
	{
		if (m_FOV >= 1.0f && m_FOV <= 45.0f)
			m_FOV -= yoffset;
		if (m_FOV <= 1.0f)
			m_FOV = 1.0f;
		if (m_FOV >= 45.0f)
			m_FOV = 45.0f;

		m_Proj = glm::perspective(glm::radians(m_FOV), 960.0f / 540.0f, 0.1f, 100.0f);
	}

	void TestNormalMaps::ProcessInput(GLFWwindow* window, float deltaTime)
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

	void TestNormalMaps::OnImGuiRender()
	{
		ImGui::Text("Welcome to the Normal Map Test Enviroment. Use WASD to move around and QE to zoom in and out so that you can see the shadows. There are more setting options below.");
		if (ImGui::CollapsingHeader("Plane Options")) {
			ImGui::SliderFloat3("Rotate Plane", &m_CubeRotation.x, -180.0f, 180.0f);
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
		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
	}
}