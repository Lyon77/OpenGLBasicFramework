#pragma once
#include "Tests/Test.h"

#include "VertexBuffer.h"
#include "VertexBufferLayout.h"
#include "Texture.h"
#include "Camera.h"
#include "FrameBuffer.h"

#include <memory>

namespace test {

	class TestBloom : public Test {
	public:
		TestBloom();
		~TestBloom();

		void OnUpdate(float deltaTime) override;
		void OnRender() override;
		void OnImGuiRender() override;
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

		//FrameBuffer
		std::unique_ptr <VertexArray> m_FrameVAO;
		std::unique_ptr <VertexBuffer> m_FrameVertexBuffer;
		std::unique_ptr <IndexBuffer> m_FrameIndexBuffer;
		std::unique_ptr <Shader> m_FrameShader;
		std::unique_ptr <Shader> m_BlurShader;
		std::unique_ptr <FrameBuffer> m_FrameBuffer;
		std::unique_ptr <FrameBuffer> m_VerticalFB;
		std::unique_ptr <FrameBuffer> m_HorizontalFB;

		glm::mat4 m_Proj, m_View;

		//Camera Information
		float m_Speed;
		float m_FOV;
		glm::vec2 m_YawPitch;

		//Mouse Calculations
		float lastX = 960.0f / 2.0f, lastY = 540.0f / 2.0f;
		bool firstMouse = true;

		//location of the lamp light
		glm::vec3 m_LampPos;
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

		float m_SpecularPower;

		bool m_AttenuationCheckbox;
		bool m_Bloom;

		float m_Exposure;
	};
}