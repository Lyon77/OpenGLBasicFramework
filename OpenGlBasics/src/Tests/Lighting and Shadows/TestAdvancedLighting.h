#pragma once
#include "Tests/Test.h"

#include "VertexBuffer.h"
#include "VertexBufferLayout.h"
#include "Texture.h"
#include "Camera.h"

#include <memory>

namespace test {

	class TestAdvancedLighting : public Test {
	public:
		TestAdvancedLighting();
		~TestAdvancedLighting();

		void OnUpdate(float deltaTime) override;
		void OnRender() override;
		void OnImGuiRender() override;
		void ProcessInput(GLFWwindow* window, float deltaTime) override;
		void MouseCallback(GLFWwindow* window, double xpos, double ypos) override;
		void ScrollCallback(GLFWwindow* window, double xoffset, double yoffset) override;
	private:
		std::unique_ptr <VertexArray> m_VAO;
		std::unique_ptr <VertexBuffer> m_VertexBuffer;
		std::unique_ptr <IndexBuffer> m_IndexBuffer;
		std::unique_ptr <Shader> m_Shader;
		std::unique_ptr <Shader> m_LampShader;

		std::unique_ptr <Texture> m_TextureDiffuse;
		std::unique_ptr <Texture> m_TextureSpecular;

		glm::mat4 m_Proj, m_View;

		//Camera Information
		std::unique_ptr <Camera> m_Camera;
		float m_Speed;
		float m_FOV;
		glm::vec2 m_YawPitch;

		//Mouse Calculations
		float lastX = 960.0f / 2.0f, lastY = 540.0f / 2.0f;
		bool firstMouse = true;

		//location of the lamp light
		glm::vec3 m_LampPos = glm::vec3(1.2f, 1.0f, 2.0f);
		glm::vec3 m_CubePos;

		glm::vec3 cubePositions[10] = {
		   glm::vec3(0.0f,  0.0f,  0.0f),
		   glm::vec3(2.0f,  5.0f, -3.0f),
		   glm::vec3(-1.5f, -2.2f, -2.5f),
		   glm::vec3(-3.8f, -2.0f, -4.3f),
		   glm::vec3(2.4f, -0.4f, -3.5f),
		   glm::vec3(-1.7f,  3.0f, -3.5f),
		   glm::vec3(1.3f, -2.0f, -2.5f),
		   glm::vec3(1.5f,  2.0f, -2.5f),
		   glm::vec3(1.5f,  0.2f, -1.5f),
		   glm::vec3(-1.3f,  1.0f, -1.5f)
		};

		//Lighting Conditions
		glm::vec3 m_CubeColor;

		glm::vec3 m_LampAmbient;
		glm::vec3 m_LampDiffuse;
		glm::vec3 m_LampSpecular;

		float m_SpecularPower;

		bool m_AttenuationCheckbox;
	};
}