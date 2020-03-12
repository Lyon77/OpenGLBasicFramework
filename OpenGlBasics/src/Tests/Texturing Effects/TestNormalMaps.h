#pragma once
#include "Tests/Test.h"

#include "VertexBuffer.h"
#include "VertexBufferLayout.h"
#include "Texture.h"
#include "FrameBuffer.h"
#include "CubeMap.h"
#include "Camera.h"

#include <memory>

namespace test {

	class TestNormalMaps : public Test {
	public:
		TestNormalMaps();
		~TestNormalMaps();

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
		std::unique_ptr <Texture> m_NormalMap;

		//FrameBuffer
		std::unique_ptr <VertexArray> m_FrameVAO;
		std::unique_ptr <VertexBuffer> m_FrameVertexBuffer;
		std::unique_ptr <IndexBuffer> m_FrameIndexBuffer;
		std::unique_ptr <Shader> m_DirShader;
		std::unique_ptr <Shader> m_PointSpotShader;
		std::unique_ptr <FrameBuffer> m_FrameBuffer;
		std::unique_ptr <CubeMap> m_FrameCubeMap;

		glm::mat4 m_Proj, m_View;

		//Camera Information
		float m_Speed;
		float m_FOV;
		glm::vec2 m_YawPitch;

		//Mouse Calculations
		float lastX = 960.0f / 2.0f, lastY = 540.0f / 2.0f;
		bool firstMouse = true;

		//location of the lamp light
		glm::vec3 m_LampPos = glm::vec3(0.2f, 0.3f, 0.5f);
		glm::vec3 m_CubeRotation;

		//Lighting Conditions
		glm::vec3 m_LampAmbient;
		glm::vec3 m_LampDiffuse;
		glm::vec3 m_LampSpecular;

		float m_SpecularPower;
	};
}