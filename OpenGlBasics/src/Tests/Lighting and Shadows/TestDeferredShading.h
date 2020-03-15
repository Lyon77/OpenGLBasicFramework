#pragma once
#include "Tests/Test.h"

#include "VertexBuffer.h"
#include "VertexBufferLayout.h"
#include "Texture.h"
#include "FrameBuffer.h"
#include "Model.h"
#include "Camera.h"

#include <memory>

namespace test {

	class TestDeferredShading : public Test {
	public:
		TestDeferredShading();
		~TestDeferredShading();

		void OnUpdate(float deltaTime) override;
		void OnRender() override;
		void OnImGuiRender() override;
		void MouseCallback(GLFWwindow* window, double xpos, double ypos) override;
		void ScrollCallback(GLFWwindow* window, double xoffset, double yoffset) override;
	private:
		std::unique_ptr <VertexArray> m_QuadVAO;
		std::unique_ptr <VertexBuffer> m_QuadVertexBuffer;
		std::unique_ptr <IndexBuffer> m_QuadIndexBuffer;
		std::unique_ptr <VertexArray> m_CubeVAO;
		std::unique_ptr <VertexBuffer> m_CubeVertexBuffer;
		std::unique_ptr <IndexBuffer> m_CubeIndexBuffer;

		std::unique_ptr <Shader> m_GBufferShader;
		std::unique_ptr <Shader> m_QuadShader;
		std::unique_ptr <Shader> m_LightingShader;
		std::unique_ptr <Shader> m_LampShader;

		std::unique_ptr <FrameBuffer> m_FBO;

		std::unique_ptr <Model> m_Model;

		glm::mat4 m_Proj, m_View;

		std::vector<glm::vec3> m_ObjectPositions;
		std::vector<glm::vec3> m_LightPositions;
		std::vector<glm::vec3> m_LightColors;

		//Camera Information
		float m_Speed;
		float m_FOV;
		glm::vec2 m_YawPitch;

		//Mouse Calculations
		float lastX = 960.0f / 2.0f, lastY = 540.0f / 2.0f;
		bool firstMouse = true;

		glm::vec3 m_LampPos = glm::vec3(0.0f, 15.0f, 0.0f);

		glm::vec3 m_LampAmbient;
		glm::vec3 m_LampDiffuse;
		glm::vec3 m_LampSpecular;

		float m_SpecularPower;

		bool m_AttenuationCheckbox;
	};
}